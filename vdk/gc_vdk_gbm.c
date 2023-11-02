/******************************************************************************\
|*                                                                            *|
|* Copyright (c) 2009 by Vivante Corp.  All rights reserved.                  *|
|*                                                                            *|
|* This program is free software: you can redistribute it and/or modify it    *|
|* under the terms of the GNU General Public License as published by the Free *|
|* Software Foundation, either version 3 of the License, or (at your option)  *|
|* any later version.                                                         *|
|*                                                                            *|
|* This program is distributed in the hope that it will be useful, but        *|
|* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY *|
|* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   *|
|* for more details.                                                          *|
|*                                                                            *|
|* You should have received a copy of the GNU General Public License along    *|
|* with this program.  If not, see <http://www.gnu.org/licenses/>.            *|
|*                                                                            *|
\******************************************************************************/

#ifndef _XOPEN_SOURCE
#  define _XOPEN_SOURCE 501
#endif

#undef EGL_API_WL

#include <gc_vdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>

#include <sys/time.h>
#include <pthread.h>

#include <gbm.h>

#ifndef DRM_DIR_NAME
#  define DRM_DIR_NAME  "/dev/dri"
#  define DRM_DEV_NAME  "%s/card%d"
#  define DRM_CONTROL_DEV_NAME  "%s/controlD%d"
#  define DRM_RENDER_DEV_NAME  "%s/renderD%d"
#endif

/*******************************************************************************
***** Version Signature *******************************************************/

const char * _VDK_PLATFORM = "\n\0$PLATFORM$GBM\n";

/* The struct of vdk resource. */
typedef struct _vdkResource
{
    vdkDisplay           display;
    void *               egl;
                         
    pthread_t            threadID;

    struct _vdkResource* next;
} *vdkResource;

/* Encapsulation of struct _vdkPrivate. */
typedef struct _vdkPrivate
{
    vdkResource vdkList;
    int32_t     threadCount;
} *vdkPrivate;

/* Initialization global variable _vdk and _vdk_mutex. */
static vdkPrivate _vdk = NULL;
pthread_mutex_t _vdk_mutex = PTHREAD_MUTEX_INITIALIZER;

/* Create vdk resource and insert it to vdk list. */
static bool vdkCreateResource(vdkPrivate priv)
{
    vdkResource vdkList = priv->vdkList;
    vdkResource vdkNew = (vdkResource)malloc(sizeof(struct _vdkResource));

    if (!vdkNew)
    {
        fprintf(stderr, "%s(%d): allocate vdkResource failed\n",
                __func__, __LINE__);
        return false;
    }

    vdkNew->display = (EGLNativeDisplayType)NULL;

#if gcdSTATIC_LINK
    vdkNew->egl = NULL;
#else
    vdkNew->egl = dlopen("libEGL.so", RTLD_LAZY);
#endif

    vdkNew->threadID = pthread_self();
    vdkNew->next = NULL;

    if (vdkList)
    {
        while (vdkList->next)
        {
            vdkList = vdkList->next;
        }

        /* Insert new vdk resource to the end of list. */
        vdkList->next = vdkNew;
    }
    else
    {
        /* Add new vdk resource as the head of the list. */
        priv->vdkList = vdkNew;
    }

    return true;
}

/* Remove the vdk resource of current thread from vdk list. */
static void vdkRemoveResource(vdkPrivate priv)
{
    vdkResource vdkPre = priv->vdkList;
    vdkResource vdkRemove = vdkPre;

    while (vdkRemove)
    {
        if (vdkRemove->threadID == pthread_self())
            break;

        vdkPre = vdkRemove;
        vdkRemove = vdkRemove->next;
    }

    if (vdkRemove)
    {
        /* If remove vdk resource is the head of the list. */
        if (vdkRemove == priv->vdkList)
        {
            priv->vdkList = vdkRemove->next;
        }
        else
        {
            vdkPre->next = vdkRemove->next;
        }

        free(vdkRemove);
    }
    else
    {
        fprintf(stderr, "%s(%d): can't the remove vdk resource of current thread\n",
                __func__, __LINE__);
    }
}

/* Find the vdk resource of current thread. */
static vdkResource vdkFindResource(vdkPrivate priv)
{
    vdkResource vdkFind = NULL;

    if (!priv)
    {
        fprintf(stderr, "%s(%d): vdkPrivate is NULL\n",
                __func__, __LINE__);
        return NULL;
    }

    vdkFind = priv->vdkList;

    while (vdkFind)
    {
        if (vdkFind->threadID == pthread_self())
            return vdkFind;

        vdkFind = vdkFind->next;
    }

    fprintf(stderr, "%s(%d): can't the vdk resource of current thread\n",
            __func__, __LINE__);
    return NULL;
}

/* Destroy whole vdk list. */
static void vdkDestroyResource(vdkPrivate priv)
{
    vdkResource vdkDestroy;
    while (priv->vdkList != NULL)
    {
        vdkDestroy = priv->vdkList;
        if (vdkDestroy)
        {
            priv->vdkList = vdkDestroy->next;
            free(vdkDestroy);
        }
    }
}

/*******************************************************************************
** Initialization.
*/

VDKAPI vdkPrivate VDKLANG
vdkInitialize(
    void
)
{
    pthread_mutex_lock(&_vdk_mutex);

    if (_vdk == NULL)
    {
        _vdk = (vdkPrivate)malloc(sizeof(struct _vdkPrivate));
        if (_vdk)
        {
            _vdk->vdkList = NULL;
            _vdk->threadCount = 0;
        }
        else
        {
            fprintf(stderr, "%s(%d): allocate _vdk failed\n",
                    __func__, __LINE__);
            pthread_mutex_unlock(&_vdk_mutex);
            return NULL;
        }
    }

    /* Create a new vdk resource to vdk list. */
    if (!vdkCreateResource(_vdk))
    {
        fprintf(stderr, "%s(%d): vdkCreateResource allocate vdkResource failed\n",
                __func__, __LINE__);
        pthread_mutex_unlock(&_vdk_mutex);
        return NULL;
    }

    _vdk->threadCount++;

    pthread_mutex_unlock(&_vdk_mutex);
    return _vdk;
}

VDKAPI void VDKLANG
vdkExit(
    vdkPrivate Private
)
{

    vdkResource vdkList = vdkFindResource(Private);

    if (!vdkList)
    {
        fprintf(stderr, "%s(%d): vdkFindResource find vdk resource failed\n",
                __func__, __LINE__);
        return;
    }

    pthread_mutex_lock(&_vdk_mutex);
    if (vdkList->egl != NULL)
    {
        dlclose(vdkList->egl);
        vdkList->egl = NULL;
    }

    _vdk->threadCount--;

    if (_vdk->threadCount != 0)
    {
        vdkRemoveResource(Private);
        pthread_mutex_unlock(&_vdk_mutex);
        return;
    }

    vdkDestroyResource(Private);
    free(Private);
    _vdk = NULL;
    pthread_mutex_unlock(&_vdk_mutex);
}

/*******************************************************************************
** Display.
*/

VDKAPI vdkDisplay VDKLANG
vdkGetDisplayByIndex(
    vdkPrivate Private,
    int DisplayIndex
    )
{
    char drmCardPath[256];
    int fd;
    vdkResource vdkList = vdkFindResource(Private);

    if (!vdkList)
    {
        fprintf(stderr, "%s(%d): vdkFindResource find vdk resource failed\n",
                __func__, __LINE__);
        return NULL;
    }

    if (vdkList->display != (EGLNativeDisplayType) NULL)
    {
        return vdkList->display;
    }

    snprintf(drmCardPath, 256, DRM_DEV_NAME, DRM_DIR_NAME, DisplayIndex);
    fd = open(drmCardPath, O_RDWR);

    if (fd < 0)
    {
        return NULL;
    }

    vdkList->display = gbm_create_device(fd);
    return vdkList->display;
}

VDKAPI vdkDisplay VDKLANG
vdkGetDisplay(
    vdkPrivate Private
    )
{
    return vdkGetDisplayByIndex(Private, 0);
}

VDKAPI int VDKLANG
vdkGetDisplayInfo(
    vdkDisplay Display,
    int * Width,
    int * Height,
    unsigned long * Physical,
    int * Stride,
    int * BitsPerPixel
    )
{
    if (!Display)
    {
        return 0;
    }

    return 0;
}

VDKAPI void VDKLANG
vdkDestroyDisplay(
    vdkDisplay Display
    )
{
    int fd = gbm_device_get_fd(Display);
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        fprintf(stderr, "%s(%d): vdkFindResource find vdk resource failed\n",
                __func__, __LINE__);
        return;
    }

    gbm_device_destroy(Display);
    close(fd);

    if (vdkList->display == Display)
    {
        vdkList->display = NULL;
    }
}

/*******************************************************************************
** Windows
*/

vdkWindow
vdkCreateWindow(
    vdkDisplay Display,
    int X,
    int Y,
    int Width,
    int Height
    )
{
    struct gbm_surface * sur;

    if (!Display)
    {
        return NULL;
    }

    sur = gbm_surface_create(Display, Width, Height, GBM_FORMAT_ARGB8888,
                             GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    return sur;
}

VDKAPI int VDKLANG
vdkGetWindowInfo(
    vdkWindow Window,
    int * X,
    int * Y,
    int * Width,
    int * Height,
    int * BitsPerPixel,
    unsigned int * Offset
    )
{
    return 0;
}

VDKAPI void VDKLANG
vdkDestroyWindow(
    vdkWindow Window
    )
{
    struct gbm_surface * sur = (struct gbm_surface *) Window;
    gbm_surface_destroy(sur);
}

VDKAPI int VDKLANG
vdkShowWindow(
    vdkWindow Window
    )
{
    return 1;
}

VDKAPI int VDKLANG
vdkHideWindow(
    vdkWindow Window
    )
{
    return 1;
}

VDKAPI void VDKLANG
vdkSetWindowTitle(
    vdkWindow Window,
    const char * Title
    )
{
}

VDKAPI void VDKLANG
vdkCapturePointer(
    vdkWindow Window
    )
{
}

/*******************************************************************************
** Events.
*/
VDKAPI int VDKLANG
vdkGetEvent(
    vdkWindow Window,
    vdkEvent * Event
    )
{
    return 0;
}

/*******************************************************************************
** EGL Support. ****************************************************************
*/

EGL_ADDRESS
vdkGetAddress(
    vdkPrivate Private,
    const char * Function
    )
{
#if gcdSTATIC_LINK
    return (EGL_ADDRESS) eglGetProcAddress(Function);
#else
    vdkResource vdkList = vdkFindResource(Private);
    if (!vdkList)
    {
        fprintf(stderr, "%s(%d): vdkFindResource find vdk resource failed\n",
                __func__, __LINE__);
        return NULL;
    }
    return (EGL_ADDRESS)dlsym(vdkList->egl, Function);
 #endif
}

/*******************************************************************************
** Time. ***********************************************************************
*/

/*
    vdkGetTicks

    Get the number of milliseconds since the system started.

    PARAMETERS:

        None.

    RETURNS:

        unsigned int
            The number of milliseconds the system has been running.
*/
VDKAPI unsigned int VDKLANG
vdkGetTicks(
    void
    )
{
    struct timeval tv;

    /* Return the time of day in milliseconds. */
    gettimeofday(&tv, 0);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

/*******************************************************************************
** Pixmaps. ********************************************************************
*/


VDKAPI vdkPixmap VDKLANG
vdkCreatePixmap(
    vdkDisplay Display,
    int Width,
    int Height,
    int BitsPerPixel
    )
{
    int format;

    if (!Display)
    {
        return NULL;
    }

    switch (BitsPerPixel)
    {
    case 16:
        format = GBM_FORMAT_RGB565;
        break;
    case 32:
        format = GBM_FORMAT_ARGB8888;
        break;
    default:
        return NULL;
    }

    return gbm_bo_create(Display, Width, Height, format, GBM_BO_USE_RENDERING);
}

VDKAPI int VDKLANG
vdkGetPixmapInfo(
    vdkPixmap Pixmap,
    int * Width,
    int * Height,
    int * BitsPerPixel,
    int * Stride,
    void ** Bits
    )
{
    if (!Pixmap)
    {
        return 0;
    }

    if (Bits)
    {
        return 0;
    }

    return 0;
}

VDKAPI void VDKLANG
vdkDestroyPixmap(
    vdkPixmap Pixmap
    )
{
    gbm_bo_destroy(Pixmap);
}

/*******************************************************************************
** ClientBuffers. **************************************************************
*/

VDKAPI vdkClientBuffer VDKLANG
vdkCreateClientBuffer(
    int Width,
    int Height,
    int Format,
    int Type
    )
{
    return NULL;
}

VDKAPI int VDKLANG
vdkGetClientBufferInfo(
    vdkClientBuffer ClientBuffer,
    int * Width,
    int * Height,
    int * Stride,
    void ** Bits
    )
{
    return 0;
}

VDKAPI int VDKLANG
vdkDestroyClientBuffer(
    vdkClientBuffer ClientBuffer
    )
{
    return 0;
}
