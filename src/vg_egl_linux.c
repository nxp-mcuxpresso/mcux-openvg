#if !defined _XOPEN_SOURCE
#define _XOPEN_SOURCE 501
#endif

#define EGL_NO_PLATFORM_SPECIFIC_TYPES 1

#include "vg_context.h"
#include "EGL/eglplatform.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <linux/fb.h>
#include <vg_lite.h>


/* Tiling modes. */
typedef enum _TILING {
    INVALIDTILED = 0x0,      /* Invalid tiling */
    /* Tiling basic modes enum'ed in power of 2. */
    LINEAR       = 0x1,      /* No    tiling. */
    TILED        = 0x2,      /* 4x4   tiling. */
} TILING;

struct _FBFunctions
{
    VGint (*OpenDevice)(VGint index, char* dev);
    VGint (*ReleaseDevice)(VGint file);
    void * (*MemoryMap)(void * start, size_t len, VGint prot, VGint flags, VGint fd, VGuint offset);
    VGint (*MemoryUnmap)(void * addr, size_t len);
    VGint (*GetFix)(VGint file, struct fb_fix_screeninfo *fixInfo);
    VGint (*GetVar)(VGint file, struct fb_var_screeninfo *varInfo);
    VGint (*SetVar)(VGint file, struct fb_var_screeninfo *varInfo);
#ifdef FBIO_WAITFORVSYNC
    VGint (*WaitForVsync)(VGint file, void * val);
#endif
    VGint (*PanDisplay)(VGint file, struct fb_var_screeninfo *varInfo);
};

/* Structure that defines a display. */
struct _FBDisplay
{
    uint32_t               signature;
    VGint                  index;
    VGint                  file;
    size_t                 physical;
    VGint                  stride;
    VGint                  width;
    VGint                  height;
    VGint                  alignedHeight;
    VGint                  bpp;
    VGint                  size;
    void *                 memory;
    struct fb_fix_screeninfo    fixInfo;
    struct fb_var_screeninfo    varInfo;
    struct fb_var_screeninfo    orgVarInfo;
    pthread_cond_t          cond;
    pthread_mutex_t         condMutex;
    VGuint                  alphaLength;
    VGuint                  alphaOffset;
    VGuint                  redLength;
    VGuint                  redOffset;
    VGuint                  greenLength;
    VGuint                  greenOffset;
    VGuint                  blueLength;
    VGuint                  blueOffset;
    VGImageFormat           format;
    TILING                  tiling;
    VGint                   refCount;
    VGboolean                 panVsync;

    struct _FBDisplay *     next;
    VGboolean               serverSide;
    struct _FBFunctions     functions;
    VGboolean               useVFB;
};

/* Structure that defines a window. */
struct _FBWindow
{
    struct _FBDisplay* display;
    VGuint            offset;
    VGint             x, y;
    VGint             width;
    VGint             height;
    VGint             safeWidth;
    VGint             safeHeight;
    VGint             swapInterval;
    /* Color format. */
    VGImageFormat     format;
};

/* Structure that defines a pixmap. */
struct _FBPixmap
{
    /* Pointer to memory bits. */
    void *       original;
    void *       bits;

    /* Bits per pixel. */
    VGint         bpp;

    /* Size. */
    VGint         width, height;
    VGint         alignedWidth, alignedHeight;
    VGint         stride;
    VGImageFormat format;
};

struct _FBMode
{
    const char* name;
    VGuint refresh;
    VGuint xres;
    VGuint yres;
    VGuint pixclock;
    VGuint left_margin;
    VGuint right_margin;
    VGuint upper_margin;
    VGuint lower_margin;
    VGuint hsync_len;
    VGuint vsync_len;
    VGuint sync;
    VGuint vmode;
    VGuint flag;
};

struct vfbdev_info {
    VGint                      file;
    struct fb_fix_screeninfo   fixInfo;
    struct fb_var_screeninfo   varInfo;
    struct _FBMode             *videoMode;
    struct vfbdev_info         *next;
    VGint                      refCount;
    Surface*                   surface;
    void *                     memory;
};

enum {
    FB_SIG_INT,
    FB_SIG_QUIT,
    FB_SIG_TERM,

    FB_SIG_NUM,
};

typedef struct
{
    struct _FBWindow*         window;
    struct _FBDisplay*        display;
} OSWindowContext;

struct eglFbPlatform
{
    int (*GetDisplay)( NativeDisplayType * display
                             );

    int (*GetDisplayByIndex)(int displayIndex,
                                    NativeDisplayType * display
                                    );

    int (*GetDisplayInfo)( NativeDisplayType display,
                                 int * width,
                                 int * height,
                                 size_t * physical,
                                 int * stride,
                                 int * bitsPerPixel
                                );

    int (*DestroyDisplay)(NativeDisplayType Display
                                );

    int (*CreateWindow)(NativeDisplayType Display,
                                int X,
                                int Y,
                                int Width,
                                int Height,
                                NativeWindowType * Window
                                );

    int (*GetWindowInfo)(NativeDisplayType Display,
                                NativeWindowType Window,
                                int * X,
                                int * Y,
                                int * Width,
                                int * Height,
                                int * BitsPerPixel,
                                unsigned int * Offset
                                );

    int (*DestroyWindow)(NativeDisplayType Display,
                             NativeWindowType Window
                            );

    int (*CreatePixmap)( NativeDisplayType Display,
                            int Width,
                            int Height,
                            int BitsPerPixel,
                            NativePixmapType * Pixmap
                            );

    int (*GetPixmapInfo)(NativeDisplayType Display,
                         NativePixmapType Pixmap,
                         int * Width,
                         int * Height,
                         VGImageFormat* Format,
                         int * BitsPerPixel,
                         int * Stride,
                         void** Bits
                        );

    int (*DestroyPixmap)(NativeDisplayType Display,
                         NativePixmapType Pixmap
                        );

};

#define gcmCC(c1, c2, c3, c4) \
(                             \
     (char)(c1)        |      \
    ((char)(c2) <<  8) |      \
    ((char)(c3) << 16) |      \
    ((char)(c4) << 24)        \
)

#define gcmALIGN(n, align) (((n) + ((align) - 1)) & ~((align) - 1))

pthread_mutex_t* mutex;
int mutex_locked = 0;
static pthread_mutex_t displayMutex;
static pthread_once_t onceControl = PTHREAD_ONCE_INIT;
static struct sigaction oldSigHandlers[FB_SIG_NUM];
static struct _FBDisplay* displayStack = NULL;
static struct vfbdev_info *vfbDevStack = NULL;
static struct eglFbPlatform* fbBackend = NULL;

static const struct _FBMode modelist[] = {
    /* 640x480 @ 60Hz */
    {NULL, 60, 640, 480, 39721, 40, 24, 32, 11, 96, 2, 0,
        FB_VMODE_NONINTERLACED},
    /* 800x600 @ 60hz */
    {NULL, 60, 800, 600, 25000, 88, 40, 23, 1, 128, 4,
        FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
        FB_VMODE_NONINTERLACED},
    /* 1024x768 @ 60Hz */
    {NULL, 60, 1024, 768, 15384, 168, 8, 29, 3, 144, 6, 0,
        FB_VMODE_NONINTERLACED},
    /* 1920x1080 @ 60Hz */
    {NULL, 60, 1920, 1080, 6734, 148, 88, 36, 4, 44, 5,
        FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
        FB_VMODE_NONINTERLACED},
    /* 1920x1200 @ 60Hz */
    {NULL, 60, 1920, 1200, 5177, 128, 336, 1, 38, 208, 3,
        FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
        FB_VMODE_NONINTERLACED},
};

static void destroyDisplays(void)
{

    pthread_mutex_lock(&displayMutex);

    while (displayStack != NULL)
    {
        struct _FBDisplay* dpy = displayStack;
        displayStack = displayStack->next;

        if (dpy->memory != 0)
        {
            dpy->functions.MemoryUnmap(dpy->memory, dpy->size);
            dpy->memory = NULL;
        }

        if (dpy->file >= 0)
        {
            pthread_cond_broadcast(&dpy->cond);
            dpy->functions.SetVar(dpy->file, &dpy->orgVarInfo);
            dpy->functions.ReleaseDevice(dpy->file);
            dpy->file = -1;
        }

         pthread_mutex_destroy(&dpy->condMutex);
         pthread_cond_destroy(&dpy->cond);
         free(dpy);
    }

    pthread_mutex_unlock(&displayMutex);
}
static void sig_handler(int signo)
{
    static int hookSEGV = 0;

    if (hookSEGV == 0)
    {
        signal(SIGSEGV, sig_handler);
        hookSEGV = 1;
    }

    destroyDisplays();

    switch (signo)
    {
    case SIGINT:
        sigaction(SIGINT,  &oldSigHandlers[FB_SIG_INT],  NULL);
        break;
    case SIGQUIT:
        sigaction(SIGQUIT, &oldSigHandlers[FB_SIG_QUIT], NULL);
        break;
    case SIGTERM:
        sigaction(SIGTERM, &oldSigHandlers[FB_SIG_TERM], NULL);
        break;
    default:
        break;
    }

    raise(signo);
}

static void
halOnExit(
    void
    )
{
    sigaction(SIGINT,  &oldSigHandlers[FB_SIG_INT],  NULL);
    sigaction(SIGQUIT, &oldSigHandlers[FB_SIG_QUIT], NULL);
    sigaction(SIGTERM, &oldSigHandlers[FB_SIG_TERM], NULL);

    destroyDisplays();
}

static void
onceInit(
    void
    )
{
    pthread_mutexattr_t mta;
    struct sigaction newSigHandler;

    /* Register atexit callback. */
    atexit(halOnExit);

    memset(&newSigHandler, 0, sizeof(struct sigaction));
    sigemptyset(&newSigHandler.sa_mask);
    newSigHandler.sa_handler = sig_handler;

    /* Register signal handler. */
    sigaction(SIGINT,  &newSigHandler, &oldSigHandlers[FB_SIG_INT]);
    sigaction(SIGQUIT, &newSigHandler, &oldSigHandlers[FB_SIG_QUIT]);
    sigaction(SIGTERM, &newSigHandler, &oldSigHandlers[FB_SIG_TERM]);

    /* Init mutex attribute. */
    pthread_mutexattr_init(&mta);
    pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);

    /* Set display mutex as recursive. */
    pthread_mutex_init(&displayMutex, &mta);
    /* Destroy mta.*/
    pthread_mutexattr_destroy(&mta);
}

static int fbfunc_OpenDevice(int index,  char* dev)
{
    unsigned char i = 0;
    int file;
    char fbDevName[256];
    char const * const fbDevicePath[] =
    {
        "/dev/fb%d",
        "/dev/graphics/fb%d",
        NULL
    };

    if (index < 0 && dev == NULL)
    {
        return -1;
    }

    /* Create a handle to the device. */
    file = -1;
    if (index >= 0 && dev == NULL)
    {
        while ((file == -1) && fbDevicePath[i])
        {
            sprintf(fbDevName, fbDevicePath[i], index);
            file = open(fbDevName, O_RDWR);
            i++;
        }
    }
    else if (index < 0 && dev != NULL)
    {
        file = open(dev, O_RDWR);
    }

    return file;
}

static int fbfunc_ReleaseDevice(int file)
{
    if (file >= 0)
    {
        close(file);
    }

    return 0;
}

static void* fbfunc_MemoryMap(
    void * start,
    size_t len,
    int prot,
    int flags,
    int fd,
    unsigned int offset)
{
    void * addr;

    if (len <= 0 || fd < 0)
    {
        return MAP_FAILED;
    }

    addr = mmap(start, len, prot, flags, fd, offset);

    return addr;
}

static int fbfunc_MemoryUnmap(void* addr, size_t len)
{
    if (addr == NULL || len <= 0)
    {
        return -1;
    }
    munmap(addr, len);
    addr = NULL;

    return 0;
}

static int fbfunc_GetFix(int file, struct fb_fix_screeninfo *fixInfo)
{
    int ret;

    if (file < 0 || fixInfo == NULL)
    {
        return -1;
    }

    ret = ioctl(file, FBIOGET_FSCREENINFO, fixInfo);

    return ret;
}

static int fbfunc_GetVar(int file, struct fb_var_screeninfo *varInfo)
{
    int ret;

    if (file < 0 || varInfo == NULL)
    {
        return -1;
    }

    ret = ioctl(file, FBIOGET_VSCREENINFO, varInfo);

    return ret;
}

static int fbfunc_SetVar(int file, struct fb_var_screeninfo *varInfo)
{
    int ret;

    if (file < 0 || varInfo == NULL)
    {
        return -1;
    }

    ret = ioctl(file, FBIOPUT_VSCREENINFO, varInfo);

    return ret;
}

#ifdef FBIO_WAITFORVSYNC
static int fbfunc_WaitForVsync(int file, void * val)
{
    int ret;

    if (file < 0)
    {
        return -1;
    }

    ret = ioctl(file, FBIO_WAITFORVSYNC, val);

    return ret;
}
#endif

static int fbfunc_PanDisplay(int file, struct fb_var_screeninfo *varInfo)
{
    int ret;

    if (file < 0 || varInfo == NULL)
    {
        return -1;
    }

    ret = ioctl(file, FBIOPAN_DISPLAY, varInfo);

    return ret;
}

static int _vfb_ParseUserMode(int index, int *width, int *height, int *bpp, int *refresh)
{
    char *m;
    int i;
    unsigned int len;
    int yres_parsed = 0, bpp_parsed = 0, refresh_parsed = 0;
    char vfbEnv[256];

    if (index < 0)
    {
        return -1;
    }

    sprintf(vfbEnv,"VFB_FRAMEBUFFER%d_MODE", index);
    m = getenv(vfbEnv);
    if (m == NULL)
    {
        *width = 640;
        *height = 480;
        *bpp = 32;
        *refresh = 60;

        return 0;
    }

    len = strlen(m);
    for (i = len - 1; i >= 0; i--)
    {
        switch(m[i])
        {
        case '@':
            len = i;
            if (!yres_parsed && !bpp_parsed && !refresh_parsed)
            {
                *refresh = strtol(&m[i + 1], NULL, 10);
                refresh_parsed = 1;
            }
            else
            {
                goto done;
            }
            break;
        case '-':
            len = i;
            if (!yres_parsed && !bpp_parsed)
            {
                *bpp = strtol(&m[i + 1], NULL, 10);
                bpp_parsed = 1;
            }
            else
            {
                goto done;
            }
            break;
        case 'x':
            if (!yres_parsed)
            {
                *height = strtol(&m[i + 1], NULL, 10);
                yres_parsed = 1;
            }
            else
            {
                goto done;
            }
            break;
        default:
            break;
        }
    }

    if (i < 0 && yres_parsed)
    {
        *width = strtol(&m[0], NULL, 10);
    }
done:
    if (!refresh_parsed)
    {
        *refresh = 60;
    }

    if (!bpp_parsed)
    {
        *bpp = 32;
    }

    return 0;
}

static struct _FBMode* vfb_FindBestMode(int width, int height, int refresh)
{
    unsigned int i;
    struct _FBMode *mode = (struct _FBMode*)&modelist[0];

    for (i = 0; i < sizeof(modelist)/sizeof(struct _FBMode); i++)
    {
        if (modelist[i].xres == width && modelist[i].yres == height &&
            modelist[i].refresh == refresh)
        {
            mode = (struct _FBMode*)&modelist[i];
            break;
        }
    }

    return mode;
}

static int vfbfunc_OpenDevice(int index, char* dev)
{
    struct vfbdev_info *vfbdev;
    int xres = 0, yres = 0, bpp = 0, refresh = 0;
    int status;
    VGImageFormat format;
    ColorDescriptor desc;

    if (index < 0)
    {
        return -1;
    }

    for (vfbdev = vfbDevStack; vfbdev != NULL; vfbdev = vfbdev->next)
    {
        if (vfbdev->file - 0xfb == index)
        {
            /* Find display.*/
            vfbdev->refCount++;
            return vfbdev->file;
        }
    }

    vfbdev = (struct vfbdev_info*)malloc (sizeof(struct vfbdev_info));
    memset(vfbdev, 0, sizeof(struct vfbdev_info));


    status = _vfb_ParseUserMode(index, &xres, &yres, &bpp, &refresh);
    if (status != 0)
    {
        return -1;
    }

    vfbdev->file = 0xfb + index;
    vfbdev->videoMode = vfb_FindBestMode(xres, yres, refresh);
    vfbdev->varInfo.xres = vfbdev->videoMode->xres;
    vfbdev->varInfo.yres = vfbdev->videoMode->yres;
    vfbdev->varInfo.pixclock = vfbdev->videoMode->pixclock;
    vfbdev->varInfo.bits_per_pixel = bpp;
    vfbdev->varInfo.vmode = vfbdev->videoMode->vmode;
    vfbdev->varInfo.nonstd = 0;
    vfbdev->varInfo.activate = FB_ACTIVATE_NOW;
    vfbdev->varInfo.accel_flags = 0;

    if (vfbdev->varInfo.vmode & FB_VMODE_CONUPDATE)
    {
        vfbdev->varInfo.vmode |= FB_VMODE_YWRAP;
    }

    vfbdev->varInfo.xoffset = 0;
    vfbdev->varInfo.yoffset = 0;

    if (vfbdev->varInfo.xres > vfbdev->varInfo.xres_virtual)
    {
        vfbdev->varInfo.xres_virtual = vfbdev->varInfo.xres;
    }

    if (vfbdev->varInfo.yres > vfbdev->varInfo.yres_virtual)
    {
        vfbdev->varInfo.yres_virtual = vfbdev->varInfo.yres;
    }

    if (vfbdev->varInfo.bits_per_pixel <= 1)
    {
        vfbdev->varInfo.bits_per_pixel = 1;
    }
    else if (vfbdev->varInfo.bits_per_pixel <= 8)
    {
        vfbdev->varInfo.bits_per_pixel = 8;
    }
    else if (vfbdev->varInfo.bits_per_pixel <= 16)
    {
        vfbdev->varInfo.bits_per_pixel = 16;
    }
    else if (vfbdev->varInfo.bits_per_pixel <= 24)
    {
        vfbdev->varInfo.bits_per_pixel = 24;
    }
    else if (vfbdev->varInfo.bits_per_pixel <= 32)
    {
        vfbdev->varInfo.bits_per_pixel = 32;
    }
    else
    {
        free(vfbdev);
        vfbdev = NULL;
        return -1;
    }

    if (vfbdev->varInfo.xres_virtual < vfbdev->varInfo.xoffset + vfbdev->varInfo.xres)
    {
        vfbdev->varInfo.xres_virtual = vfbdev->varInfo.xoffset + vfbdev->varInfo.xres;
    }
    if (vfbdev->varInfo.yres_virtual < vfbdev->varInfo.yoffset + vfbdev->varInfo.yres)
    {
        vfbdev->varInfo.yres_virtual = vfbdev->varInfo.yoffset + vfbdev->varInfo.yres;
    }

    switch(vfbdev->varInfo.bits_per_pixel)
    {
    case 1:
    case 8:
        vfbdev->varInfo.red.offset = 0;
        vfbdev->varInfo.red.length = 8;
        vfbdev->varInfo.green.offset = 0;
        vfbdev->varInfo.green.length = 8;
        vfbdev->varInfo.blue.offset = 0;
        vfbdev->varInfo.blue.length = 8;
        vfbdev->varInfo.transp.offset = 0;
        vfbdev->varInfo.transp.length = 0;
        break;

    case 16:
        vfbdev->varInfo.red.offset = 0;
        vfbdev->varInfo.red.length = 5;
        vfbdev->varInfo.green.offset = 5;
        vfbdev->varInfo.green.length = 6;
        vfbdev->varInfo.blue.offset = 11;
        vfbdev->varInfo.blue.length = 5;
        vfbdev->varInfo.transp.offset = 0;
        vfbdev->varInfo.transp.length = 0;
        break;

    case 24:
        vfbdev->varInfo.red.offset = 0;
        vfbdev->varInfo.red.length = 8;
        vfbdev->varInfo.green.offset = 8;
        vfbdev->varInfo.green.length = 8;
        vfbdev->varInfo.blue.offset = 16;
        vfbdev->varInfo.blue.length = 8;
        vfbdev->varInfo.transp.offset = 0;
        vfbdev->varInfo.transp.length = 0;
        break;

    case 32:
        vfbdev->varInfo.red.offset = 0;
        vfbdev->varInfo.red.length = 8;
        vfbdev->varInfo.green.offset = 8;
        vfbdev->varInfo.green.length = 8;
        vfbdev->varInfo.blue.offset = 16;
        vfbdev->varInfo.blue.length = 8;
        vfbdev->varInfo.transp.offset = 24;
        vfbdev->varInfo.transp.length = 8;
        break;

    default:
        vfbdev->varInfo.red.offset = 0;
        vfbdev->varInfo.red.length = 8;
        vfbdev->varInfo.green.offset = 8;
        vfbdev->varInfo.green.length = 8;
        vfbdev->varInfo.blue.offset = 16;
        vfbdev->varInfo.blue.length = 8;
        vfbdev->varInfo.transp.offset = 24;
        vfbdev->varInfo.transp.length = 8;
        break;
    }
    vfbdev->varInfo.red.msb_right = 0;
    vfbdev->varInfo.green.msb_right = 0;
    vfbdev->varInfo.blue.msb_right = 0;
    vfbdev->varInfo.transp.msb_right = 0;
    vfbdev->varInfo.left_margin = vfbdev->videoMode->left_margin;
    vfbdev->varInfo.right_margin = vfbdev->videoMode->right_margin;
    vfbdev->varInfo.upper_margin = vfbdev->videoMode->upper_margin;
    vfbdev->varInfo.lower_margin = vfbdev->videoMode->lower_margin;
    vfbdev->varInfo.sync = vfbdev->videoMode->sync;
    vfbdev->varInfo.hsync_len = vfbdev->videoMode->hsync_len;
    vfbdev->varInfo.vsync_len = vfbdev->videoMode->vsync_len;

    sprintf(vfbdev->fixInfo.id,"VFB%d", index);
    vfbdev->fixInfo.type = FB_TYPE_PACKED_PIXELS;

    switch(vfbdev->varInfo.bits_per_pixel)
    {
    case 1:
        vfbdev->fixInfo.visual = FB_VISUAL_MONO01;
        break;

    case 8:
        vfbdev->fixInfo.visual = FB_VISUAL_PSEUDOCOLOR;
        break;

    case 16:
    case 24:
    case 32:
        vfbdev->fixInfo.visual = FB_VISUAL_TRUECOLOR;
        break;

    default:
        vfbdev->fixInfo.visual = FB_VISUAL_TRUECOLOR;
        break;
    }

    vfbdev->fixInfo.xpanstep = 0;
    vfbdev->fixInfo.ypanstep = 0;
    vfbdev->fixInfo.ywrapstep = 0;
    vfbdev->fixInfo.accel = FB_ACCEL_NONE;
    vfbdev->fixInfo.line_length = ((vfbdev->varInfo.xres_virtual * bpp + 31) & ~31) >> 3;
    vfbdev->fixInfo.smem_len = vfbdev->fixInfo.line_length * vfbdev->varInfo.yres_virtual;

    vfbdev->next = vfbDevStack;
    vfbDevStack = vfbdev;
    vfbdev->refCount++;

    if (vfbdev->varInfo.bits_per_pixel <= 16)
    {
        format = VG_sRGB_565;
    }
    else if(vfbdev->varInfo.bits_per_pixel == 24)
    {
        format = VG_sXRGB_8888;
    }
    else
    {
        format = VG_sARGB_8888;
    }

    desc = formatToDescriptor(format);

    vfbdev->surface = createSurface(&desc,vfbdev->varInfo.xres_virtual , vfbdev->varInfo.yres_virtual, 1);

    if (vfbdev->surface)
    {
        return vfbdev->file;
    }
    else
    {
        if (vfbdev)
        {
            if (vfbdev->surface)
            {
                destroySurface(vfbdev->surface);
                vfbdev->surface = NULL;
            }
            free(vfbdev);
            vfbdev = NULL;
        }
        return -1;
    }
}

static int vfbfunc_ReleaseDevice(int file)
{
    struct vfbdev_info *vfbdev;

    for (vfbdev = vfbDevStack; vfbdev != NULL; vfbdev = vfbdev->next)
    {
        if (vfbdev->file == file)
        {
            /* Find display.*/
            vfbdev->refCount--;
            if (!vfbdev->refCount)
            {
                if (vfbdev == vfbDevStack)
                {
                    vfbDevStack = vfbdev->next;
                }
                else
                {
                    struct vfbdev_info *prev = vfbDevStack;

                    while (prev->next != vfbdev)
                    {
                        prev = prev->next;
                    }
                    prev->next = vfbdev->next;
                }

                destroySurface(vfbdev->surface);
                free(vfbdev);
                vfbdev = NULL;

                return 0;
            }
        }
    }

    return 0;
}

static void* vfbfunc_MemoryMap(
    void* start,
    size_t len,
    int prot,
    int flags,
    int fd,
    unsigned int offset)
{
    void * addr = MAP_FAILED;
    struct vfbdev_info *vfbdev = NULL;

    if (len <= 0 || fd < 0)
    {
        return MAP_FAILED;
    }

    for (vfbdev = vfbDevStack; vfbdev != NULL; vfbdev = vfbdev->next)
    {
        if (vfbdev->file == fd)
        {
            addr = vfbdev->memory + offset;
            break;
        }
    }

    return addr;
}

static int vfbfunc_MemoryUnmap(void* addr, size_t len)
{
    return 0;
}

static int vfbfunc_GetFix(int file, struct fb_fix_screeninfo *fixInfo)
{
    struct vfbdev_info *vfbdev = NULL;

    if (file < 0 || fixInfo == NULL)
    {
        return -1;
    }

    for (vfbdev = vfbDevStack; vfbdev != NULL; vfbdev = vfbdev->next)
    {
        if (vfbdev->file == file)
        {
            *fixInfo = vfbdev->fixInfo;
            return 0;
        }
    }

    return -1;
}

static int vfbfunc_GetVar(int file, struct fb_var_screeninfo *varInfo)
{
    struct vfbdev_info *vfbdev = NULL;

    if (file < 0 || varInfo == NULL)
    {
        return -1;
    }

    for (vfbdev = vfbDevStack; vfbdev != NULL; vfbdev = vfbdev->next)
    {
        if (vfbdev->file == file)
        {
            *varInfo = vfbdev->varInfo;
            return 0;
        }
    }

    return -1;
}

static int vfbfunc_SetVar(int file, struct fb_var_screeninfo *varInfo)
{
    struct vfbdev_info *vfbdev = NULL;

    if (file < 0 || varInfo == NULL)
    {
        return -1;
    }

    for (vfbdev = vfbDevStack; vfbdev != NULL; vfbdev = vfbdev->next)
    {
        if (vfbdev->file == file)
        {
            vfbdev->varInfo = *varInfo;
            switch(vfbdev->varInfo.bits_per_pixel)
            {
            case 1:
                vfbdev->fixInfo.visual = FB_VISUAL_MONO01;
                break;
            case 8:
                vfbdev->fixInfo.visual = FB_VISUAL_PSEUDOCOLOR;
                break;
            case 16:
            case 24:
            case 32:
                vfbdev->fixInfo.visual = FB_VISUAL_TRUECOLOR;
                break;
            default:
                vfbdev->fixInfo.visual = FB_VISUAL_TRUECOLOR;
                break;
            }
            vfbdev->fixInfo.line_length = ((vfbdev->varInfo.xres_virtual *
                                            vfbdev->varInfo.bits_per_pixel + 31) & ~31) >> 3;
            return 0;
        }
    }

    return -1;
}

#ifdef FBIO_WAITFORVSYNC
static int vfbfunc_WaitForVsync(int file, void* val)
{
    return 0;
}
#endif

static int vfbfunc_PanDisplay(int file, struct fb_var_screeninfo *varInfo)
{
    return 0;
}

static int getFBFunctions(struct _FBDisplay* display)
{
    char *vdev, *dev;
    char fbEnv[256];
    char vfbEnv[256];

    if (display == NULL)
    {
        return -1;
    }

    display->useVFB = VG_FALSE;
    sprintf(fbEnv,"FB_FRAMEBUFFER_%d", display->index);
    dev = getenv(fbEnv);
    sprintf(vfbEnv,"VFB_ENABLE");
    vdev = getenv(vfbEnv);

    if (dev && vdev)
    {
        printf("cannot enable both FB%d and VFB simultaneously.", display->index);
        return -1;
    }

    if (dev)
    {
        display->useVFB = VG_FALSE;
    }
    else if (vdev)
    {
        unsigned int p = strtol(vdev, NULL, 10);

        if (p != 0)
        {
            display->useVFB = VG_TRUE;
        }
        else
        {
            display->useVFB = VG_FALSE;
        }
    }

    if (!display->useVFB)
    {
        display->functions.OpenDevice = fbfunc_OpenDevice;
        display->functions.ReleaseDevice = fbfunc_ReleaseDevice;
        display->functions.MemoryMap = fbfunc_MemoryMap;
        display->functions.MemoryUnmap = fbfunc_MemoryUnmap;
        display->functions.GetFix = fbfunc_GetFix;
        display->functions.GetVar = fbfunc_GetVar;
        display->functions.SetVar = fbfunc_SetVar;
#ifdef FBIO_WAITFORVSYNC
        display->functions.WaitForVsync = fbfunc_WaitForVsync;
#endif
        display->functions.PanDisplay = fbfunc_PanDisplay;
    }
    else
    {
        display->functions.OpenDevice = vfbfunc_OpenDevice;
        display->functions.ReleaseDevice = vfbfunc_ReleaseDevice;
        display->functions.MemoryMap = vfbfunc_MemoryMap;
        display->functions.MemoryUnmap = vfbfunc_MemoryUnmap;
        display->functions.GetFix = vfbfunc_GetFix;
        display->functions.GetVar = vfbfunc_GetVar;
        display->functions.SetVar = vfbfunc_SetVar;
#ifdef FBIO_WAITFORVSYNC
        display->functions.WaitForVsync = vfbfunc_WaitForVsync;
#endif
        display->functions.PanDisplay = vfbfunc_PanDisplay;
    }

    return 0;
}

int fbdev_GetDisplayByIndex(int display_id, NativeDisplayType* display_type)
{
    char *dev, *p;
    char fbDevName[256];
    struct _FBDisplay* display = NULL;
    int error = -1;
    int i = 0;

    pthread_once(&onceControl, onceInit);
    pthread_mutex_lock(&displayMutex);

    do
    {
        if (display < 0)
        {
            printf("Invalid display id. \n");
            pthread_mutex_unlock(&displayMutex);
            return -1;
        }

        display = (struct _FBDisplay *)malloc(sizeof(struct _FBDisplay));
        if (display == NULL)
        {
            printf("allocate display fail. \n");
            pthread_mutex_unlock(&displayMutex);
            return -1;
        }

        display->signature  = gcmCC('F', 'B', 'D', 'V');
        display->index      = display_id;
        display->memory     = NULL;
        display->file       = -1;
        display->tiling     = LINEAR;
        display->serverSide = EGL_FALSE;

        getFBFunctions(display);

        sprintf(fbDevName, "FB_FRAMEBUFFER_%d", display_id);
        dev = getenv(fbDevName);

        if (dev != NULL)
        {
            display->file = display->functions.OpenDevice(-1, dev);
        }

        if (display->file < 0)
        {
            display->file = display->functions.OpenDevice(display_id, NULL);
        }

        if (display->file < 0)
        {
            break;
        }

        error = display->functions.GetVar(display->file, &display->varInfo);
        if (error < 0)
        {
            break;
        }

        display->orgVarInfo = display->varInfo;
        display->alignedHeight = display->varInfo.yres;

        error = display->functions.GetVar(display->file, &display->varInfo);
        if (error < 0)
        {
            break;
        }

        error = display->functions.GetFix(display->file, &display->fixInfo);
        if (error < 0)
        {
            break;
        }

        display->physical = display->fixInfo.smem_start;
        display->stride   = display->fixInfo.line_length;
        display->size     = display->fixInfo.smem_len;
        display->width    = display->varInfo.xres;
        display->height   = display->varInfo.yres;
        display->bpp      = display->varInfo.bits_per_pixel;

        switch (display->varInfo.green.length)
        {
            case 4:
                if (display->varInfo.blue.offset == 0)
                {
                    display->format = VG_sARGB_4444;
                }
                else
                {
                    display->format = VG_sABGR_4444;
                }
                break;

            case 5:
                if (display->varInfo.blue.offset == 0)
                {
                    display->format = VG_sRGBA_5551;
                }
                else
                {
                    display->format = VG_sBGRA_5551;
                }
                break;

            case 6:
                display->format = VG_sRGB_565;
                break;

            case 8:
                if (display->varInfo.blue.offset == 0)
                {
                    display->format = VG_sARGB_8888;
                }
                else
                {
                    display->format = VG_sABGR_8888;
                }
                break;

            default:
                display->format = VG_IMAGE_FORMAT_FORCE_SIZE;
                break;
        }

        if (display->format == VG_IMAGE_FORMAT_FORCE_SIZE)
        {
            error = EGL_BAD_DISPLAY;
            break;
        }

        display->alphaLength = display->varInfo.transp.length;
        display->alphaOffset = display->varInfo.transp.offset;

        display->redLength   = display->varInfo.red.length;
        display->redOffset   = display->varInfo.red.offset;

        display->greenLength = display->varInfo.green.length;
        display->greenOffset = display->varInfo.green.offset;

        display->blueLength  = display->varInfo.blue.length;
        display->blueOffset  = display->varInfo.blue.offset;

        display->refCount = 0;

        display->panVsync = EGL_TRUE;

        display->memory = display->functions.MemoryMap(0, display->size, 
                                      PROT_READ| PROT_WRITE, MAP_SHARED, display->file, 0);

        if (display->memory == MAP_FAILED)
        {
            error = EGL_BAD_DISPLAY;
            printf("Map frame buffer memory fail. \n");
            break;
        }

        pthread_cond_init(&(display->cond), NULL);
        pthread_mutex_init(&display->condMutex, NULL);
        *display_type = (NativeDisplayType)display;

        display->refCount++;
        pthread_mutex_unlock(&displayMutex);
        return EGL_SUCCESS;
    }while(0);

    pthread_mutex_unlock(&displayMutex);

    if (display != NULL)
    {
        if (display->memory != NULL)
        {
            display->functions.MemoryUnmap(display->memory, display->size);
            display->memory = NULL;
        }

        if (display->file > 0)
        {
            display->functions.SetVar(display->file, &display->orgVarInfo);
            display->functions.ReleaseDevice(display->file);
        }

        free(display);
        display = NULL;
    }

    display = NULL;
    return error;

}

int fbdev_GetDisplay(NativeDisplayType* display)
{
    return fbdev_GetDisplayByIndex(0, display);
}

int fbdev_GetDisplayInfo(       NativeDisplayType display, int * width, int * height,
                                 size_t * physical, int * stride, int * bitsPerPixel)
{
    struct _FBDisplay* dpy = (struct _FBDisplay*)display;

    if (width)
        *width = dpy->width;

    if (height)
        *height = dpy->height;

    if (physical)
        *physical = dpy->physical;

    if (stride)
        *stride = dpy->stride;

    if (bitsPerPixel)
        *bitsPerPixel = dpy->bpp;

    return 0;
}

int fbdev_DestroyDisplay( NativeDisplayType display)
{

    struct _FBDisplay* dpy;

    dpy = (struct _FBDisplay*)display;

    pthread_mutex_lock(&displayMutex);
    if (dpy)
    {
         dpy->refCount--;

         if(dpy->refCount)
         {
             pthread_mutex_unlock(&displayMutex);
             return 0;
         }

         if (dpy->memory != NULL)
         {
             dpy->functions.MemoryUnmap(dpy->memory, dpy->size);
             dpy->memory = NULL;
         }

         if (dpy->file >= 0)
         {
            dpy->functions.SetVar(dpy->file, &dpy->orgVarInfo);
            dpy->functions.ReleaseDevice(dpy->file);
            dpy->file = -1;
         }

         pthread_mutex_destroy(&dpy->condMutex);
         pthread_cond_destroy(&dpy->cond);
         free(dpy);
    }
    pthread_mutex_unlock(&displayMutex);

    return 0;
}

int fbdev_CreateWindow(NativeDisplayType display, int x, int y, int width, int height, NativeWindowType *window)
{
    struct _FBDisplay* dpy;
    int ignoreDisplaySize = 0;
    char* p;

    dpy = (struct _FBDisplay*)display;

    if (dpy == NULL)
    {
        printf("Invalid display. \n");
        return -1;
    }

    if (width == 0)
    {
        width = dpy->width;
    }

    if (height == 0)
    {
        height = dpy->height;
    }

    if (x == -1)
    {
        x = ((dpy->width - width) / 2) & ~15;
    }

    if (y == -1)
    {
        y = ((dpy->height - height) / 2) & ~7;
    }

    if (x < 0)
        x = 0;

    if (y < 0)
        y = 0;

    p = getenv("FB_IGNORE_DISPLAY_SIZE");
    if (p != NULL)
    {
        ignoreDisplaySize = atoi(p);
    }

    if (!ignoreDisplaySize)
    {
        if ((x + width) > dpy->width)
        {
            width = dpy->width - x;
         }

        if ((y + height) > dpy->height)
        {
            height = dpy->height - y;
        }
    }

    do
    {
        struct _FBWindow* win;

        win = (struct _FBWindow*)malloc(sizeof(struct _FBWindow));
        if (win == NULL)
        {
            printf("create window instance fail. \n");
            return -1;
        }

        win->offset = y * dpy->stride + x * ((dpy->bpp + 7) / 8);
        win->display = dpy;
        win->format = dpy->format;
        win->x = x;
        win->y = y;

        win->safeWidth = win->width = width;
        win->safeHeight= win->height = height;

        if ((dpy->tiling = LINEAR) && ((x != 0) || (y != 0)))
        {
            VGint lastFbH = 0;
            VGint alignedW = width;
            VGint alignedH = height;
            VGint wAlignment = 16;
            VGint hAlignment = 4 * 2;

            alignedW = gcmALIGN(width, wAlignment);
            alignedH = gcmALIGN(height, hAlignment);

            lastFbH = y + alignedH;

            if ((x + alignedW) > dpy->width)
            {
                lastFbH += 1;
            }

            if ((lastFbH > dpy->height) && (alignedH > hAlignment))
            {
                win->safeHeight = alignedH - hAlignment;
            }
        }

        win->swapInterval = 1;

        *window = (NativeWindowType)win;
    }while(0);

    return 0;
}

int fbdev_GetWindowInfo(NativeDisplayType display,       NativeWindowType window,  int* x, int* y,
                                int* width, int* height, int* bitsPerPixel, unsigned int* offset)
{
    struct _FBWindow* win = (struct _FBWindow*)window;

    if (win == NULL)
    {
        printf("Get window info fail! \n");
        return -1;
    }
    
    if (x != NULL)
        *x = win->x;

    if (y != NULL)
        *y = win->y;

    if (width != NULL)
        *width = win->width;

    if (height != NULL)
        *height = win->height;

    if (bitsPerPixel != NULL)
        *bitsPerPixel = win->display->bpp;

    if (offset != NULL)
        *offset = win->offset;

    return 0;
}


int fbdev_DestroyWindow( NativeDisplayType display, NativeWindowType window)
{
    if (window != NULL)
    {
        free(window);
    }

    return 0;
}

int fbdev_GetWindowSize(NativeDisplayType display, NativeWindowType window, int* width, int* height)
{
    struct _FBWindow* win;

    win = (struct _FBWindow*) window;

    if (win == NULL)
    {
        printf("invalid window! \n");
        return -1;
    }

    if (width != NULL)
    {
        *width = win->width;
    }

    if (height != NULL)
    {
        *height = win->height;
    }

    return 0;
}

int fbdev_CreatePixmap(NativeDisplayType display, int width, int height, int bitsPerPixel, NativePixmapType* pixmap)
{
    VGImageFormat format = VG_sARGB_8888;
    struct _FBPixmap *pp;
    vg_lite_buffer_t *imgbuf = NULL;

    do
    {
        pp = (struct _FBPixmap *)malloc(sizeof(struct _FBPixmap));

        if (pp)
        {
            memset(pp, 0, sizeof(struct _FBPixmap));
        }

        if (bitsPerPixel <= 16)
        {
            format = VG_sRGB_565;
        }
        else if (bitsPerPixel == 24)
        {
             format = VG_sXRGB_8888;
        }
        else 
        {
            format = VG_sARGB_8888;
        }

        imgbuf = (vg_lite_buffer_t*)malloc(sizeof(vg_lite_buffer_t));
        if (imgbuf)
        {
            memset(imgbuf, 0, sizeof(vg_lite_buffer_t));
        }
        else
        {
            free(imgbuf);
            return -1;
        }

        imgbuf->width = width;
        imgbuf->height = height;
        imgbuf->format = format;
        vg_lite_allocate(imgbuf);

        pp->bits = imgbuf->memory;
        pp->stride = imgbuf->stride;
        pp->bpp = 32;
    }while(0);

    *pixmap = (NativePixmapType)pp;
     free(imgbuf);

    return 0;
}

static int fbdev_GetPixmapInfo(NativeDisplayType display, NativePixmapType pixmap, int* width, int* height, VGImageFormat* format,
                         int* bitsPerPixel, int* stride, void** bits)
{
    struct _FBPixmap* pmap = (struct _FBPixmap*)pixmap;

    if (pmap == NULL)
    {
        printf("Invalid pixmap value!\n");
        return -1;
    }

    if (width)
        *width = (int)pmap->width;

    if (height)
        *height = (int)pmap->height;

    if (format)
        *format = pmap->format;

    if (bitsPerPixel)
        *bitsPerPixel = (int)pmap->bpp;

    if (stride)
        *stride = (int)pmap->stride;

    if (bits)
        *bits = (void*)pmap->bits;

    return 0;

}

static int fbdev_DestroyPixmap(NativeDisplayType display, NativePixmapType pixmap)
{
    struct _FBPixmap* pmap = (struct _FBPixmap*)pixmap;

    if (pmap != NULL)
    {
        if (pmap->bits)
            free(pmap->bits);

        free(pmap);
        pmap = NULL;
        pixmap = NULL;
    }

    return 0;
}

static struct eglFbPlatform fbdevBackend =
{
    fbdev_GetDisplay,
    fbdev_GetDisplayByIndex,
    fbdev_GetDisplayInfo,
    fbdev_DestroyDisplay,
    fbdev_CreateWindow,
    fbdev_GetWindowInfo,
    fbdev_DestroyWindow,
    fbdev_CreatePixmap,
    fbdev_GetPixmapInfo,
    fbdev_DestroyPixmap
};

struct eglFbPlatform* getFbDevBackend()
{
    return &fbdevBackend;
}

static void
fbGetBackends(
   struct eglFbPlatform ** fbBackend
    )
{
    if (*fbBackend == NULL)
    {
        *fbBackend = getFbDevBackend();
    }
}

void *
fbGetDisplay()
{
    NativeDisplayType display = NULL;
    fbGetBackends(&fbBackend);
    fbBackend->GetDisplay(&display);
    return display;
}

void *
fbGetDisplayByIndex(
    int DisplayIndex
    )
{
    NativeDisplayType display = NULL;
    fbGetBackends(&fbBackend);
    fbBackend->GetDisplayByIndex(DisplayIndex, &display);
    return display;
}

void
fbGetDisplayGeometry(
     void * Display,
    int* Width,
    int* Height
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->GetDisplayInfo((void *)Display, Width, Height, NULL, NULL, NULL);
}

void
fbGetDisplayInfo(
    void * Display,
    int* Width,
    int* Height,
    unsigned long *Physical,
    int* Stride,
    int* BitsPerPixel
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->GetDisplayInfo(Display, Width, Height,(size_t*)Physical, Stride, BitsPerPixel);
}

void
fbDestroyDisplay(
     void * Display
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->DestroyDisplay(Display);
}

void *
fbCreateWindow(
    void * display,
    int x,
    int y,
    int width,
    int height
    )
{
    NativeWindowType window = NULL;
    fbGetBackends(&fbBackend);
    fbBackend->CreateWindow((NativeDisplayType)display, x, y, width, height, &window);
    return window;
}

void
fbGetWindowGeometry(
    void * window,
    int* x,
    int* y,
    int* width,
    int* height
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->GetWindowInfo(NULL, (void *)window, x, y, width, height, NULL, NULL);
}

void
fbGetWindowInfo(
    void * window,
    int* x,
    int* y,
    int* width,
    int* height,
    int* bitsPerPixel,
     unsigned int* offset
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->GetWindowInfo(NULL, (NativeWindowType)window, x, y, width, height, bitsPerPixel, offset);
}

void
fbDestroyWindow(
    void * window
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->DestroyWindow(NULL, (NativeWindowType)window);
}

void *
fbCreatePixmap(
    void* display,
    int width,
    int height
    )
{
    NativePixmapType pixmap = NULL;
    fbGetBackends(&fbBackend);
    fbBackend->CreatePixmap((NativeDisplayType)display, width, height, 32, &pixmap);
    return pixmap;
}

void *
fbCreatePixmapWithBpp(
    void * display,
    int width,
    int height,
    int bitsPerPixel
    )
{
    NativePixmapType pixmap = NULL;
    fbGetBackends(&fbBackend);
    fbBackend->CreatePixmap((NativeDisplayType)display, width, height, bitsPerPixel, &pixmap);
    return pixmap;
}

void
fbGetPixmapGeometry(
    void* pixmap,
    int* width,
    int* height
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->GetPixmapInfo(NULL, (NativePixmapType)pixmap, width, height, NULL, NULL, NULL, NULL);
}

void
fbGetPixmapInfo(
    void* pixmap,
    int* width,
    int* height,
    VGImageFormat *format,
    int* bitsPerPixel,
    int* stride,
    void** bits
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->GetPixmapInfo(NULL, (NativeDisplayType)pixmap, width, height, format, bitsPerPixel, stride, bits);
}

void
fbDestroyPixmap(
    void * Pixmap
    )
{
    fbGetBackends(&fbBackend);
    fbBackend->DestroyPixmap(NULL, (void *)Pixmap);
}

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

VGEGLDisplay* OSGetDisplay(NativeDisplayType display_id)
{
    VGEGLDisplay* vdisplay = NULL;

    fbdev_GetDisplayByIndex(0, &display_id);

    vdisplay = (VGEGLDisplay*)malloc(sizeof(VGEGLDisplay));
    memset(vdisplay, 0, sizeof(VGEGLDisplay));

    vdisplay->m_id = display_id;
    vdisplay->nativeDisplay = display_id;

    return vdisplay;
}

void* OSCreateWindowContext(NativeWindowType window)
{
    OSWindowContext *ctx = NULL;

    ctx = (OSWindowContext *)malloc(sizeof(OSWindowContext));

    if (ctx)
    {
        ctx->window = (struct _FBWindow *)window;
        ctx->display = ctx->window->display;
    }
    else
    {
        printf("Create window context fail! \n");
    }

    return ctx;
}

void OSDestroyWindowContext(void* context)
{
    OSWindowContext* ctx = (OSWindowContext *)context;

    if (ctx)
    {
        fbDestroyWindow(ctx->window);
        fbDestroyDisplay(ctx->display);
    }
}

VGboolean OSIsWindow(const void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;

    if (ctx->window)
    {
        return EGL_TRUE;
    }
    else
    {
        return EGL_FALSE;
    }
}

void OSGetWindowSize(const void* context, int* width, int* height)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    fbGetWindowInfo(ctx->window, NULL, NULL, width, height, NULL, NULL);
}

Drawable* OSFbCreateWindowDrawable(NativeWindowType window, ColorDescriptor desc, int samples , VGint maskbits, int index)
{
    struct _FBWindow * win;
    struct _FBDisplay *dpy;
    Drawable* draw = NULL;
    Image* image = NULL;
    vg_lite_buffer_t *imgbuf = NULL;
    int offset;

    win = (struct _FBWindow *)window;
    dpy = win->display;

    imgbuf = (vg_lite_buffer_t*)malloc(sizeof(vg_lite_buffer_t));
    if (imgbuf)
    {
       memset(imgbuf, 0, sizeof(vg_lite_buffer_t));
    }
    else
    {
        printf("Alloc vg lite buffer fail \n");
        goto ErrorImabuf;
    }

    offset = dpy->stride* dpy->alignedHeight * index;

    imgbuf->width  = dpy->width;
    imgbuf->height = dpy->height;
    imgbuf->format = vgFormatTovgliteFormat(dpy->format);
    imgbuf->memory = dpy->memory;
    imgbuf->address = dpy->physical + offset ;
    imgbuf->stride = dpy->stride;

    image = (Image*)malloc(sizeof(Image));
    if (image)
    {
        memset(image, 0, sizeof(Image));
    }
    else
    {
        printf("Alloc Image fail \n");
        goto ErrorImage;
    }
    
    image->m_vglbuf = (void*)imgbuf;
    image->m_data = (VGubyte*)imgbuf->memory;
    image->m_allowedQuality = 0;

    image->m_width  = win->width;
    image->m_height = win->height;
    image->m_desc   = desc;
    image->m_stride = dpy->stride;
    image->m_storageOffsetX = win->x;
    image->m_storageOffsetY = win->y;

    draw = createImageDrawable(image, maskbits);
    return draw;

ErrorImage:
    free(image);

ErrorImabuf:
    free(imgbuf);

   return NULL;
}

void OSBlitToWindow(void* context, const Drawable* drawable)
{
    OSWindowContext* ctx = (OSWindowContext*)context;
    Image * im = drawable->m_color->m_image;
    struct _FBWindow* win = ctx->window;
    struct _FBDisplay* display = ctx->display;

    pthread_mutex_lock(&displayMutex);

    {
       int swapInterval = win->swapInterval;
        if (swapInterval != 0 || !display->panVsync)
        {
            pthread_mutex_lock(&display->condMutex);

#ifdef FBIO_WAITFORVSYNC
            if (display->panVsync)
            {
                swapInterval--;
            }

            while (swapInterval--)
            {
                display->functions.WaitForVsync(display->file, (void *)0);
            }
#endif
            display->varInfo.xoffset = im->m_storageOffsetX;
            display->varInfo.yoffset = im->m_storageOffsetY;
            display->varInfo.activate = FB_ACTIVATE_VBL;
            display->functions.PanDisplay(display->file, &display->varInfo);
            pthread_cond_broadcast(&display->cond);
            pthread_mutex_unlock(&display->condMutex);
        }
    }

    pthread_mutex_unlock(&displayMutex);
}

VGuint OSGetPixmapInfo(NativePixmapType pixmap, VGuint* width, VGuint* height, VGImageFormat* format, VGuint* bitsPerPixel, VGuint* stride, VGubyte** bits)
{
    fbGetPixmapInfo(pixmap, width,height, format, bitsPerPixel, stride, (void**)bits);

    return 0;
}
