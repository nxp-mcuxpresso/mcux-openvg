#if !defined _XOPEN_SOURCE
#   define _XOPEN_SOURCE 501
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <assert.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-egl-backend.h>
#include <vg_lite.h>
#include "vg_context.h"
#include "EGL/eglplatform.h"

struct Display {
    struct wl_display* dpy;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct wl_shm* shm;
    VGboolean has_xrgb;
};

struct Buffer{
    struct wl_buffer* buffer;
    void* shm_data;
    int width, height;
    uint32_t format;
    int busy;
};

struct Surface
{
    int width, height;
    struct Buffer buffer[2];
    struct wl_surface* surface;
    struct wl_callback* callback;
};


typedef struct
{
    struct wl_egl_window*      window;
    struct Display* display;
    struct wl_registry* registry;
    struct wl_compositor* compositor;
    struct xdg_wm_base* *wm_base;
    struct Surface surf;
} OSWindowContext;

struct wl_egl_pixmap
{
    /* Parameters. */
    int         width;
    int         height;
    VGImageFormat         format;

    /* Underlying information. */
    int         stride;
    void*       data;
    VGuint      phy_address;
    VGint         bpp;
    vg_lite_buffer_t* vglbuf;
};


pthread_mutex_t *mutex = NULL;
VGuint mutex_locked = 0;


void* OSGetCurrentThreadID(void)
{
   return (void*) pthread_self();
}

void OSAcquireMutex(void)
{
    pthread_mutexattr_t mta;
    int error;

    if (mutex == NULL)
    {
        mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
        pthread_mutexattr_init(&mta);
        pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(mutex, &mta);
        pthread_mutexattr_destroy(&mta);
        mutex_locked = 0;
    }

    error = pthread_mutex_lock(mutex);

    switch (error)
    {
        case 0:
            mutex_locked = 1;
            break;

        case EBUSY:
            mutex_locked = 0;
            printf("timout \n");
            break;

        case EINVAL:
            mutex_locked = 0;
            printf("invalid mutex value \n");
            break;

        case EDEADLK:
            mutex_locked = 0;
            printf("deadlock situation \n");
            break;
        default:
            mutex_locked = 0;
            printf("acqure mutex error:%d \n", error);
    }

}

void OSReleaseMutex(void)
{
    int error;

    if (mutex == NULL)
    {
        return;
    }

    if (mutex_locked)
    {
        error = pthread_mutex_unlock(mutex);

        switch (error)
        {
            case 0:
                mutex_locked = 0;
                break;

            case EINVAL:
                printf("Trying to free a invalid mutex value. \n");
                break;

            case EPERM:
                printf("Trying to free a mutex not locked by itself \n");
                break;

            default:
                printf("mutex unlock error:%d \n", error);
        }
    }
    else
    {
        printf("Tring fo free a mutex not locked! \n");
    }
    
}

void OSDeinitMutex(void)
{
    int error;

    if (mutex == NULL)
        return;

    error = pthread_mutex_destroy(mutex);

    switch (error)
    {
        case 0:
            mutex = NULL;
            break;

        case EBUSY:
            printf("Try destroy a mutex locked. \n");
            break;

        case EINVAL:
            printf("Try destroy a invalid mutex value. \n");
            break;

        default:
            printf("Destroy a mutex fail, error:%d. \n", error);
    }

}

void shm_format(void* data, struct wl_shm* wl_shm, uint32_t format)
{
    struct Display *dpy = data;

    if (format == WL_SHM_FORMAT_XRGB8888)
        dpy->has_xrgb = VG_TRUE;
}

struct wl_shm_listener shm_listener = {
    shm_format
};

static void  registry_handle_global(void*data, struct wl_registry *registry, uint32_t id, const char* interface, uint32_t version)
{
    struct Display *d = data;

    if (strcmp(interface, "wl_compositor") == 0){
        d->compositor = wl_registry_bind(registry, id, &wl_compositor_interface, 1);
    }
    else if (strcmp(interface, "wl_shm") == 0)
    {
        d->shm = wl_registry_bind(registry, id, &wl_shm_interface, 1);
        wl_shm_add_listener(d->shm, &shm_listener, d);
    }
}

static void registry_handle_global_remove(void* data, struct wl_registry* registry, uint32_t name)
{
    
}

static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    registry_handle_global_remove
};

EGLDisplay OSGetDisplay(EGLNativeDisplayType display_id)
{
    VGEGLDisplay* display;
    struct Display* native_display;

    display_id = wl_display_connect(NULL);

    if (display_id == NULL)
        return NULL;

    native_display = malloc(sizeof(struct Display));
    if (native_display == NULL)
    {
        printf("alloc native display fail:%s\n", strerror(errno));
        return NULL;
    }

    memset(native_display, 0, sizeof(struct Display));

    native_display->dpy = display_id;
    native_display->registry = wl_display_get_registry(display_id);

    wl_registry_add_listener(native_display->registry, &registry_listener, native_display);
    wl_display_roundtrip(native_display->dpy);

    if (native_display->shm == NULL)
    {
        printf(" No wl_shm global \n");
        return NULL;
    }

    wl_display_roundtrip(native_display->dpy);
    display = (VGEGLDisplay*)malloc(sizeof(VGEGLDisplay));
    display->m_id = display_id;

    display->nativeDisplay = native_display;
    return display;
}

int os_fd_set_cloexec(int fd)
{
    long flags;

    if (fd == -1)
        return -1;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
        return -1;

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
        return -1;

    return 0;
}

static int set_cloexec_or_close(int fd)
{
    if (os_fd_set_cloexec(fd) != 0) {
        close(fd);
        return -1;
    }
    return fd;
}

int os_socketpair_cloexec(int domain, int type, int protocol, int *sv)
{
    int ret;

#ifdef SOCK_CLOEXEC
    ret = socketpair(domain, type | SOCK_CLOEXEC, protocol, sv);
    if (ret == 0 || errno != EINVAL)
        return ret;
#endif

    ret = socketpair(domain, type, protocol, sv);
    if (ret < 0)
        return ret;

    sv[0] = set_cloexec_or_close(sv[0]);
    sv[1] = set_cloexec_or_close(sv[1]);

    if (sv[0] != -1 && sv[1] != -1)
        return 0;

    close(sv[0]);
    close(sv[1]);
    return -1;
}

int os_epoll_create_cloexec(void)
{
    int fd;

#ifdef EPOLL_CLOEXEC
    fd = epoll_create1(EPOLL_CLOEXEC);
    if (fd >= 0)
        return fd;
    if (errno != EINVAL)
        return -1;
#endif

    fd = epoll_create(1);
    return set_cloexec_or_close(fd);
}

static int create_tmpfile_cloexec(char *tmpname)
{
    int fd;

#ifdef HAVE_MKOSTEMP
    fd = mkostemp(tmpname, O_CLOEXEC);
    if (fd >= 0)
        unlink(tmpname);
#else
    fd = mkstemp(tmpname);
    if (fd >= 0) {
        fd = set_cloexec_or_close(fd);
        unlink(tmpname);
    }
#endif

    return fd;
}

/*
 * Create a new, unique, anonymous file of the given size, and
 * return the file descriptor for it. The file descriptor is set
 * CLOEXEC. The file is immediately suitable for mmap()'ing
 * the given size at offset zero.
 *
 * The file should not have a permanent backing store like a disk,
 * but may have if XDG_RUNTIME_DIR is not properly implemented in OS.
 *
 * The file name is deleted from the file system.
 *
 * The file is suitable for buffer sharing between processes by
 * transmitting the file descriptor over Unix sockets using the
 * SCM_RIGHTS methods.
 *
 * If the C library implements posix_fallocate(), it is used to
 * guarantee that disk space is available for the file at the
 * given size. If disk space is insufficient, errno is set to ENOSPC.
 * If posix_fallocate() is not supported, program may receive
 * SIGBUS on accessing mmap()'ed file contents instead.
 */
int os_create_anonymous_file(off_t size)
{
    static const char template[] = "/weston-shared-XXXXXX";
    const char *path;
    char *name;
    int fd;
    int ret;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        errno = ENOENT;
        return -1;
    }

    name = malloc(strlen(path) + sizeof(template));
   if (!name)
        return -1;

    strcpy(name, path);
    strcat(name, template);

    fd = create_tmpfile_cloexec(name);

    free(name);

    if (fd < 0)
        return -1;

#ifdef HAVE_POSIX_FALLOCATE
    do {
        ret = posix_fallocate(fd, 0, size);
    } while (ret == EINTR);

    if (ret != 0) {
        close(fd);
        errno = ret;
        return -1;
    }
#else
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(fd);
        return -1;
    }
#endif

    return fd;
}

#ifndef HAVE_STRCHRNUL
char *
strchrnul(const char *s, int c)
{
    while (*s && *s != c)
        s++;

    return (char *)s;
}
#endif

static void buffer_release(void* data, struct wl_buffer* buffer)
{
    struct Buffer *buf = data;
    buf->busy = 0;
}

static const struct wl_buffer_listener buffer_listener = {
    buffer_release
};

static int create_shm_buffer(struct Display* display, struct Buffer * buffer, int width, int height, uint32_t format)
{
    struct wl_shm_pool* pool;
    int fd, size, stride;
    void* data;

    stride = width * 4;
    size = stride * height;

    fd = os_create_anonymous_file(size);

    if (fd < 0)
    {
        printf("creating a buffer file for %d B failed: %s \n", size, strerror(errno));
        return -1;
    }

    data = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    if (data == MAP_FAILED)
    {
        printf("mmap failed:%s \n", strerror(errno));
        close(fd);
        return -1;
    }

    pool = wl_shm_create_pool(display->shm, fd, size);
    buffer->buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, format);

    wl_buffer_add_listener(buffer->buffer, &buffer_listener, buffer);
    wl_shm_pool_destroy(pool);
    close(fd);

    buffer->shm_data = data;
    buffer->width = width;
    buffer->height = height;
    buffer->format = format;

    return 0;
}

void* OSCreateWindowContext(EGLNativeWindowType window, VGEGLDisplay display)
{
    OSWindowContext* ctx = NULL;
    struct Display* nDisplay;
    struct wl_display* wl_dpy;
    int ret;

    nDisplay = display.nativeDisplay;
    wl_dpy = display.m_id;

   if (nDisplay == NULL)
    {
        nDisplay = malloc(sizeof(struct Display));
        
        nDisplay->dpy = wl_dpy;
        nDisplay->registry = wl_display_get_registry(wl_dpy);

        wl_registry_add_listener(nDisplay->registry, &registry_listener, nDisplay);
        wl_display_roundtrip(nDisplay->dpy);

        if (nDisplay->shm == NULL)
        {
            printf(" No wl_shm global \n");
            return NULL;
        }
    }

    ctx = (OSWindowContext*)malloc(sizeof(OSWindowContext));

    ctx->window = (struct wl_egl_window*)window;
    ctx->display = nDisplay;

    ctx->surf.width = window->width;
    ctx->surf.height = window->height;
    ctx->surf.surface = window->surface;

    ret = create_shm_buffer(ctx->display, &(ctx->surf.buffer[0]),
                              window->width, window->height, WL_SHM_FORMAT_XRGB8888);
    if (ret < 0)
    {
        printf("Create window buffer failt:%s\n", strerror(errno));
        return NULL;
    }

    ctx->surf.buffer[0].busy = 0;
    memset(ctx->surf.buffer[0].shm_data, 0xFF, window->width * window->height *4);

    ret = create_shm_buffer(ctx->display, &(ctx->surf.buffer[1]),
                               window->width, window->height, WL_SHM_FORMAT_XRGB8888);
    if (ret < 0)
    {
        printf("Create window buffer failt:%s\n", strerror(errno));
        return NULL;
    }

    ctx->surf.buffer[1].busy = 0;
    memset(ctx->surf.buffer[1].shm_data, 0xFF, window->width * window->height *4);

    
    return ctx;
}

void OSDestroyWindowContext(void* context)
{
    OSWindowContext *ctx;
    struct Display* display;

    ctx = (OSWindowContext*)context;
    display = ctx->display;

    if (display->shm)
        wl_shm_destroy(display->shm);

    wl_registry_destroy(display->registry);
    wl_display_flush(display->dpy);
    wl_display_disconnect(display->dpy);

    if (ctx)
    {
        free(ctx);
    }
}

VGboolean OSIsWindow(const void* context)
{
    OSWindowContext* ctx;

    ctx = (OSWindowContext*)context;

    if (ctx->window)
        return VG_TRUE;
    else
        return VG_FALSE;
}

void OSGetWindowSize(const void* context, int* width, int* height)
{
    OSWindowContext* ctx;

    ctx = (OSWindowContext*)context;

    if (width)
        *width = ctx->window->width;

    if (height)
        *height = ctx->window->height;

}

void OSBlitToWindow(void* context, const Drawable* drawable)
{
    OSWindowContext* ctx = (OSWindowContext*) context;
    struct Surface* surf = &(ctx->surf);
    struct Buffer buffer;

    vg_lite_buffer_t* vglbuf = drawable->m_color->m_image->m_vglbuf;

    if (!surf->buffer[0].busy)
        buffer = surf->buffer[0];
    else if (!surf->buffer[1].busy)
        buffer = surf->buffer[1];

    memcpy(buffer.shm_data, vglbuf->memory, vglbuf->stride * vglbuf->height);
    wl_surface_attach(surf->surface, buffer.buffer, 0, 0);
    wl_surface_damage(surf->surface, 0, 0, buffer.width, buffer.height);
    wl_surface_commit(surf->surface);
    buffer.busy = 1;
    wl_display_flush(ctx->display->dpy);
}

VGuint OSGetPixmapInfo(EGLNativePixmapType pixmap, VGuint* width, VGuint* height, VGImageFormat* format,
                                 VGuint* bitsPerPixel, VGuint* stride, VGubyte** bits)
{
    struct wl_egl_pixmap* wl_pixmap;

    wl_pixmap = (struct wl_egl_pixmap*) pixmap;

    if (width != NULL)
        *width = wl_pixmap->width;

    if (height != NULL)
        *height = wl_pixmap->height;

    if (format != NULL)
        *format = wl_pixmap->format;

    if (bitsPerPixel != NULL)
        *bitsPerPixel = wl_pixmap->bpp;

    if (stride != NULL)
        *stride = wl_pixmap->stride;

    if (*bits != NULL)
        *bits = wl_pixmap->data;

}

static int create_pixmap_content(struct wl_egl_pixmap *pixmap)
{
    VGImageFormat format;
    ColorDescriptor colordesc;
    vg_lite_buffer_t* vglbuf;
    vg_lite_error_t vgerr;

    switch (pixmap->format)
    {
    case WL_SHM_FORMAT_XRGB4444:
        format = VG_sARGB_4444;
        break;
    case WL_SHM_FORMAT_XBGR4444:
        format = VG_sABGR_4444;
        break;
    case WL_SHM_FORMAT_ARGB4444:
        format = VG_sARGB_4444;
        break;
    case WL_SHM_FORMAT_ABGR4444:
        format = VG_sABGR_4444;
        break;
    case WL_SHM_FORMAT_XRGB1555:
        format = VG_sARGB_1555;
        break;
    case WL_SHM_FORMAT_XBGR1555:
        format = VG_sABGR_1555;
        break;
    case WL_SHM_FORMAT_ARGB1555:
        format = VG_sARGB_1555;
        break;
    case WL_SHM_FORMAT_ABGR1555:
        format = VG_sABGR_1555;
        break;
    case WL_SHM_FORMAT_RGB565:
        format = VG_sRGB_565;
        break;
    case WL_SHM_FORMAT_BGR565:
        format = VG_sBGR_565;
        break;
    case WL_SHM_FORMAT_XRGB8888:
        format = VG_sXRGB_8888;
        break;
    case WL_SHM_FORMAT_XBGR8888:
        format = VG_sXBGR_8888;
        break;
    case WL_SHM_FORMAT_ARGB8888:
        format = VG_sARGB_8888;
        break;
    case WL_SHM_FORMAT_ABGR8888:
        format = VG_sABGR_8888;
        break;

    default:
        printf("%s: format not supported - 0x%x\n",
                __func__, pixmap->format);
        return -EINVAL;
    }

    vglbuf = (vg_lite_buffer_t*)malloc(sizeof(vg_lite_buffer_t));
    if (vglbuf)
    {
        memset(vglbuf, 0, sizeof(vg_lite_buffer_t));
    }
    else
    {
        free(vglbuf);
        return -1;
    }
    
    vglbuf->width = pixmap->width;
    vglbuf->height = pixmap->height;
    vglbuf->format = vgFormatTovgliteFormat(format);
    vgerr = vg_lite_allocate(vglbuf);

    if (vgerr != VG_LITE_SUCCESS)
    {
        free(vglbuf);
        return -1;
    }

    colordesc = formatToDescriptor(format);

    pixmap->bpp = colordesc.bitsPerPixel;
    pixmap->stride = vglbuf->width * (colordesc.bitsPerPixel + 7)/8;
    pixmap->data    = vglbuf->memory;
    pixmap->phy_address = vglbuf->address;
    pixmap->vglbuf = vglbuf;

    return 0;

}

struct wl_egl_pixmap *wl_egl_pixmap_create(int width, int height, int format)
{
    struct wl_egl_pixmap *pixmap;

    if (width <= 0 || height <= 0)
        return NULL;

    pixmap = malloc(sizeof(struct wl_egl_pixmap));
    if (pixmap == NULL)
    {
        return NULL;
    }

    pixmap->width     = width;
    pixmap->height    = height;
    pixmap->format    = format;

    pixmap->stride   = 0;
    pixmap->data     = 0;
    pixmap->phy_address = 0;
    pixmap->vglbuf = NULL;

    if (create_pixmap_content(pixmap) != 0)
    {
        free(pixmap);
        pixmap = 0;
    }

    return pixmap;
}

void wl_egl_pixmap_destroy(struct wl_egl_pixmap *pixmap)
{
    vg_lite_buffer_t * vglbuf;

    vglbuf = pixmap->vglbuf;

    if (vglbuf)
    {
        vg_lite_free(vglbuf);
        pixmap->vglbuf = NULL;
    }

    if (pixmap)
    {
        free(pixmap);
        pixmap = NULL;
    }
}


void wl_egl_pixmap_get_stride(struct wl_egl_pixmap *pixmap, int *stride)
{
    if (stride)
        *stride = pixmap->stride;
}

void wl_egl_pixmap_lock(struct wl_egl_pixmap *pixmap, void **pixels)
{
    if (pixels)
        *pixels = pixmap->data;
}

void wl_egl_pixmap_unlock(struct wl_egl_pixmap *pixmap)
{
}


