/****************************************************************************
*
*    Copyright 2022 Vivante Corporation, Santa Clara, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "VDK/gc_vdk.h"

/*******************************************************************************
** Data type *******************************************************************
*/
typedef void * SURFACE;
typedef int SURF_FORMAT;
typedef int SURF_TYPE;
typedef void * OS;
#define IS_SUCCESS(func) (func == 0)


/*******************************************************************************
** API part.********************************************************************
*/
typedef int (* _GAL_GetDisplayByIndex) (int DisplayIndex, EGLNativeDisplayType *,void *);
typedef int (* _GAL_GetDisplayInfo) (EGLNativeDisplayType, int*, int*, unsigned long *,int*,int*);
typedef int (* _GAL_DestroyDisplay) (EGLNativeDisplayType);

/*******************************************************************************
** Windows. ********************************************************************
*/
typedef int (* _GAL_CreateWindow) (EGLNativeDisplayType,int,int,int,int,   EGLNativeWindowType *);
typedef int (* _GAL_DestroyWindow) (EGLNativeDisplayType,EGLNativeWindowType);
typedef int (* _GAL_GetWindowInfoEx) (EGLNativeDisplayType,EGLNativeWindowType,int*,int*,int*,int*,int*, unsigned int*, SURF_FORMAT *, SURF_TYPE *);

/*******************************************************************************
** Pixmaps. ********************************************************************
*/
typedef int (* _GAL_CreatePixmap) (EGLNativeDisplayType,int,int,int,   EGLNativePixmapType *);
typedef int (* _GAL_GetPixmapInfo) (EGLNativeDisplayType,EGLNativePixmapType,int*,int*,int*,int*, void**);
typedef int (* _GAL_DestroyPixmap) (EGLNativeDisplayType,EGLNativePixmapType);

/*******************************************************8/9/2012 11:40:45 AM************************
** OS relative. ****************************************************************
*/
typedef int (* _GAL_LoadEGLLibrary) (void**);
typedef int (* _GAL_FreeEGLLibrary) (void *);
typedef int (* _GAL_ShowWindow) (EGLNativeDisplayType,EGLNativeWindowType);
typedef int (* _GAL_HideWindow) (EGLNativeDisplayType,EGLNativeWindowType);
typedef int (* _GAL_SetWindowTitle) (EGLNativeDisplayType,EGLNativeWindowType,const char *);
typedef int (* _GAL_CapturePointer) (EGLNativeDisplayType,EGLNativeWindowType);
typedef int (* _GAL_GetEvent) (EGLNativeDisplayType,EGLNativeWindowType, void *);
typedef int (* _GAL_CreateClientBuffer) (int,int,int,int,void**);
typedef int (* _GAL_GetClientBufferInfo) (void *,int*,int*,int*,void**);
typedef int (* _GAL_DestroyClientBuffer) (void *);
typedef int (* _GAL_GetProcAddress) (OS,void *,const char *,void**);
    
/*----- Time -----------------------------------------------------------------*/
/* Get the number of milliseconds since the system started.
*/
typedef unsigned int (* _GAL_GetTicks) (  void);

typedef struct _GAL_API {
    _GAL_GetDisplayByIndex      GAL_GetDisplayByIndex;
    _GAL_GetDisplayInfo         GAL_GetDisplayInfo;
    _GAL_DestroyDisplay         GAL_DestroyDisplay;
    _GAL_CreateWindow           GAL_CreateWindow;
    _GAL_DestroyWindow          GAL_DestroyWindow;
    _GAL_GetWindowInfoEx        GAL_GetWindowInfoEx;
    _GAL_CreatePixmap           GAL_CreatePixmap;
    _GAL_GetPixmapInfo          GAL_GetPixmapInfo;
    _GAL_DestroyPixmap          GAL_DestroyPixmap;
    _GAL_LoadEGLLibrary         GAL_LoadEGLLibrary;
    _GAL_FreeEGLLibrary         GAL_FreeEGLLibrary;
    _GAL_ShowWindow             GAL_ShowWindow;
    _GAL_HideWindow             GAL_HideWindow;
    _GAL_SetWindowTitle         GAL_SetWindowTitle;
    _GAL_CapturePointer         GAL_CapturePointer;
    _GAL_GetEvent               GAL_GetEvent;
    _GAL_CreateClientBuffer     GAL_CreateClientBuffer;
    _GAL_GetClientBufferInfo    GAL_GetClientBufferInfo;
    _GAL_DestroyClientBuffer    GAL_DestroyClientBuffer;
    _GAL_GetProcAddress         GAL_GetProcAddress;
    _GAL_GetTicks               GAL_GetTicks;
} GAL_API;

extern int gcoOS_LoadEGLLibrary();
extern int gcoOS_FreeEGLLibrary();
extern int gcoOS_GetDisplayByIndex();
extern int gcoOS_GetDisplayInfo();
extern int gcoOS_DestroyDisplay();
extern int gcoOS_CreateWindow();
extern int gcoOS_DestroyWindow();
extern int gcoOS_GetWindowInfoEx();
extern int gcoOS_HideWindow();
extern int gcoOS_CreatePixmap();
extern int gcoOS_GetPixmapInfo();
extern int gcoOS_DestroyPixmap();
extern int gcoOS_LoadEGLLibrary();
extern int gcoOS_FreeEGLLibrary();
extern int gcoOS_ShowWindow();
extern int gcoOS_SetWindowTitle();
extern int gcoOS_CapturePointer();
extern int gcoOS_GetEvent();
extern int gcoOS_CreateClientBuffer();
extern int gcoOS_GetClientBufferInfo();
extern int gcoOS_DestroyClientBuffer();
extern int gcoOS_GetProcAddress();
extern int gcoOS_GetTicks();

GAL_API * GAL = NULL;

int HAL_Constructor() {
    GAL = (GAL_API *) malloc(sizeof(GAL_API));
    if (GAL == NULL)
    {
        printf("Allocate memory failed\n");
        return 1;
    }

    /*******************************************************************************
    ** Display. ********************************************************************
    */
    GAL->GAL_GetDisplayByIndex = (_GAL_GetDisplayByIndex)gcoOS_GetDisplayByIndex;
    GAL->GAL_GetDisplayInfo = (_GAL_GetDisplayInfo)gcoOS_GetDisplayInfo;
    GAL->GAL_DestroyDisplay = (_GAL_DestroyDisplay)gcoOS_DestroyDisplay;

    /*******************************************************************************
    ** Windows. ********************************************************************
    */
    GAL->GAL_CreateWindow = (_GAL_CreateWindow)gcoOS_CreateWindow;
    GAL->GAL_DestroyWindow = (_GAL_DestroyWindow)gcoOS_DestroyWindow;
    GAL->GAL_GetWindowInfoEx = (_GAL_GetWindowInfoEx)gcoOS_GetWindowInfoEx;

    /*******************************************************************************
    ** Pixmaps. ********************************************************************
    */
    GAL->GAL_CreatePixmap = (_GAL_CreatePixmap)gcoOS_CreatePixmap;
    GAL->GAL_GetPixmapInfo = (_GAL_GetPixmapInfo)gcoOS_GetPixmapInfo;
    GAL->GAL_DestroyPixmap = (_GAL_DestroyPixmap)gcoOS_DestroyPixmap;

    /*******************************************************************************
    ** OS relative. ****************************************************************
    */
    GAL->GAL_LoadEGLLibrary = (_GAL_LoadEGLLibrary)gcoOS_LoadEGLLibrary;
    GAL->GAL_FreeEGLLibrary = (_GAL_FreeEGLLibrary)gcoOS_FreeEGLLibrary;
    GAL->GAL_ShowWindow = (_GAL_ShowWindow)gcoOS_ShowWindow;
    GAL->GAL_HideWindow = (_GAL_HideWindow)gcoOS_HideWindow;
    GAL->GAL_SetWindowTitle = (_GAL_SetWindowTitle)gcoOS_SetWindowTitle;
    GAL->GAL_CapturePointer = (_GAL_CapturePointer)gcoOS_CapturePointer;
    GAL->GAL_GetEvent = (_GAL_GetEvent)gcoOS_GetEvent;
    GAL->GAL_CreateClientBuffer = (_GAL_CreateClientBuffer)gcoOS_CreateClientBuffer;
    GAL->GAL_GetClientBufferInfo = (_GAL_GetClientBufferInfo)gcoOS_GetClientBufferInfo;
    GAL->GAL_DestroyClientBuffer = (_GAL_DestroyClientBuffer)gcoOS_DestroyClientBuffer;
    GAL->GAL_GetProcAddress = (_GAL_GetProcAddress)gcoOS_GetProcAddress;

    /*----- Time -----------------------------------------------------------------*/
    /* Get the number of milliseconds since the system started.
    */
    GAL->GAL_GetTicks = (_GAL_GetTicks)gcoOS_GetTicks;

    return 0;
}

void HAL_Destructor()
{
    if (GAL != NULL)
    {
        free(GAL);
    }
}

/*******************************************************************************
***** Version Signature *******************************************************/

const char * _VDK_PLATFORM = "\n\0$PLATFORM$GAL\n";

/* The struct of vdk resource. */
typedef struct _vdkResource
{
    vdkDisplay           display;
    void *               egl;
         
    int                  threadID;

    struct _vdkResource* next;
} *vdkResource;

/* Encapsulation of struct _vdkPrivate. */
typedef struct _vdkPrivate
{
    vdkResource vdkList;
    int     threadCount;
} *vdkPrivate;



int
gcoOS_GetCurrentThreadID(
    void
)
{
    return (int)(GetCurrentThreadId());
}

void * PLS_accessLock = NULL;

void
gcoOS_LockPLS(
    void
)
{
    if (PLS_accessLock)
    {
        WaitForSingleObject((HANDLE)PLS_accessLock, INFINITE);
    }
}

void
gcoOS_UnLockPLS(
    void
)
{
    if (PLS_accessLock)
    {
        ReleaseMutex((HANDLE)PLS_accessLock);

    }
}


/*******************************************************************************
** Private functions.
*/
static vdkPrivate _vdk = NULL;

/* Create vdk resource and insert it to vdk list. */
static int vdkCreateResource(vdkPrivate priv)
{
    vdkResource vdkList = priv->vdkList;
    vdkResource vdkNew = (vdkResource)malloc(sizeof(struct _vdkResource));

    if (!vdkNew)
    {
        printf("Allocate vdkResource failed\n");
        return 0;
    }

    vdkNew->display = (EGLNativeDisplayType)NULL;

    if (GAL->GAL_LoadEGLLibrary(&vdkNew->egl))
    {
        free(vdkNew);
        return 0;
    }

    vdkNew->threadID = gcoOS_GetCurrentThreadID();
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

    return 1;
}

/* Remove the vdk resource of current thread from vdk list. */
static void vdkRemoveResource(vdkPrivate priv)
{
    vdkResource vdkPre = priv->vdkList;
    vdkResource vdkRemove = vdkPre;

    while (vdkRemove)
    {
        if (vdkRemove->threadID == gcoOS_GetCurrentThreadID())
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
        printf("Can't the remove vdk resource of current thread\n");
    }
}

/* Find the vdk resource of current thread. */
static vdkResource vdkFindResource(vdkPrivate priv)
{
    vdkResource vdkFind = NULL;

    if (!priv)
    {
        printf("vdkPrivate is NULL\n");
        return NULL;
    }

    vdkFind = priv->vdkList;

    while (vdkFind)
    {
        if (vdkFind->threadID == gcoOS_GetCurrentThreadID())
            return vdkFind;

        vdkFind = vdkFind->next;
    }

    printf("Can't the vdk resource of current thread\n");
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
    if (HAL_Constructor())
        return _vdk;

    gcoOS_LockPLS();

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
            printf("Allocate _vdk failed\n");
            gcoOS_UnLockPLS();
            return NULL;
        }
    }

    /* Create a new vdk resource to vdk list. */
    if (!vdkCreateResource(_vdk))
    {
        printf("vdkCreateResource allocate vdkResource failed\n");
        gcoOS_UnLockPLS();
        return NULL;
    }

    _vdk->threadCount++;

    gcoOS_UnLockPLS();
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
        printf("vdkFindResource find vdk resource failed\n");
        return;
    }

    gcoOS_LockPLS();

    if (vdkList->egl != NULL)
    {
        GAL->GAL_FreeEGLLibrary(vdkList->egl);
    }

    _vdk->threadCount--;
    HAL_Destructor();

    if (_vdk->threadCount != 0)
    {
        vdkRemoveResource(Private);
        gcoOS_UnLockPLS();
        
        return;
    }

    vdkDestroyResource(Private);
    free(Private);
    _vdk = NULL;
    gcoOS_UnLockPLS();
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
    vdkDisplay  display = (EGLNativeDisplayType) NULL;
    vdkResource vdkList = vdkFindResource(Private);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return NULL;
    }

    if (vdkList->display != (EGLNativeDisplayType) NULL)
    {
        return vdkList->display;
    }

    if (IS_SUCCESS(GAL->GAL_GetDisplayByIndex(DisplayIndex, &display, NULL)))
    {
        vdkList->display = display;
    }
    
    return display;
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
    return IS_SUCCESS(GAL->GAL_GetDisplayInfo(Display,
                                              Width, Height,
                                              Physical,
                                              Stride,
                                              BitsPerPixel)) ? 1 : 0;
}

VDKAPI void VDKLANG
vdkDestroyDisplay(
    vdkDisplay Display
    )
{
    GAL->GAL_DestroyDisplay(Display);
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
    vdkWindow   window;

    return IS_SUCCESS(GAL->GAL_CreateWindow(Display,
                                            X, Y,
                                            Width, Height,
                                            (EGLNativeWindowType *) &window)) ? window : 0;
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
    SURF_FORMAT  format;
    SURF_TYPE type;

    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return 0;
    }

    return IS_SUCCESS(GAL->GAL_GetWindowInfoEx(vdkList->display,
                                               Window,
                                               X, Y,
                                               Width, Height,
                                               BitsPerPixel,
                                               Offset,
                                               &format,
                                               &type)) ? 1 : 0;
}

VDKAPI void VDKLANG
vdkDestroyWindow(
    vdkWindow Window
    )
{
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return;
    }

    GAL->GAL_DestroyWindow(vdkList->display, Window);
}

VDKAPI int VDKLANG
vdkShowWindow(
    vdkWindow Window
    )
{
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return 0;
    }

    return IS_SUCCESS(GAL->GAL_ShowWindow(vdkList->display, Window)) ? 1 : 0;
}

VDKAPI int VDKLANG
vdkHideWindow(
    vdkWindow Window
    )
{
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return 0;
    }

    return IS_SUCCESS(GAL->GAL_HideWindow(vdkList->display, Window)) ? 1 : 0;
}

VDKAPI void VDKLANG
vdkSetWindowTitle(
    vdkWindow Window,
    const char * Title
    )
{
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return;
    }

    GAL->GAL_SetWindowTitle(vdkList->display, Window, Title);
}

VDKAPI void VDKLANG
vdkCapturePointer(
    vdkWindow Window
    )
{
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return;
    }

    GAL->GAL_CapturePointer(vdkList->display, Window);
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
#if 0 /* No Event support */
    halEvent halEvent;
    int result = 0;
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return result;
    }

    if (IS_SUCCESS(GAL->GAL_GetEvent(vdkList->display, Window, &halEvent)))
    {
        result = 1;
        switch(halEvent.type)
        {
        case HAL_KEYBOARD:
            Event->type = VDK_KEYBOARD;
            Event->data.keyboard.scancode = (vdkKeys)halEvent.data.keyboard.scancode;
            Event->data.keyboard.pressed  = halEvent.data.keyboard.pressed;
            Event->data.keyboard.key      = halEvent.data.keyboard.key;
            break;
        case HAL_BUTTON:
            Event->type = VDK_BUTTON;
            Event->data.button.left     = halEvent.data.button.left;
            Event->data.button.right    = halEvent.data.button.right;
            Event->data.button.middle   = halEvent.data.button.middle;
            Event->data.button.x        = halEvent.data.button.x;
            Event->data.button.y        = halEvent.data.button.y;
            break;
        case HAL_POINTER:
            Event->type = VDK_POINTER;
            Event->data.pointer.x = halEvent.data.pointer.x;
            Event->data.pointer.y = halEvent.data.pointer.y;
            break;
        case HAL_CLOSE:
            Event->type = VDK_CLOSE;
            break;
        case HAL_WINDOW_UPDATE:
            Event->type = VDK_WINDOW_UPDATE;
            break;
        default:
            return 0;
        }
    }
    return result;
#else
    return 0;
#endif
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
    return (EGL_ADDRESS)NULL;
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
    return GAL->GAL_GetTicks();
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
    vdkPixmap   pixmap;

    if (IS_SUCCESS(GAL->GAL_CreatePixmap(Display,
                                         Width, Height,
                                         BitsPerPixel,
                                         (EGLNativePixmapType *) &pixmap)))
    {
        return pixmap;
    }

    return 0;
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
    int bpp, stride;
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return 0;
    }

    if (IS_SUCCESS(GAL->GAL_GetPixmapInfo(vdkList->display,
                                          Pixmap,
                                          Width, Height,
                                          &bpp,
                                          &stride,
                                          Bits)))
    {
        if (BitsPerPixel != NULL)
            *BitsPerPixel = bpp;
        if (Stride != NULL)
            *Stride = stride;

        return 1;
    }
    else
    {
        return 0;
    }
}

VDKAPI void VDKLANG
vdkDestroyPixmap(
    vdkPixmap Pixmap
    )
{
    vdkResource vdkList = vdkFindResource(_vdk);

    if (!vdkList)
    {
        printf("vdkFindResource find vdk resource failed\n");
        return;
    }

    GAL->GAL_DestroyPixmap(vdkList->display, Pixmap);
}


/*******************************************************************************
** ClientBuffers. **************************************************************
*/

/* GL_VIV_direct_texture */
#ifndef GL_VIV_direct_texture
#define GL_VIV_YV12                        0x8FC0
#define GL_VIV_NV12                        0x8FC1
#define GL_VIV_YUY2                        0x8FC2
#define GL_VIV_UYVY                        0x8FC3
#define GL_VIV_NV21                        0x8FC4
#endif

VDKAPI vdkClientBuffer VDKLANG
vdkCreateClientBuffer(
    int Width,
    int Height,
    int Format,
    int Type
    )
{
    vdkClientBuffer clientBuffer;

    if (IS_SUCCESS(GAL->GAL_CreateClientBuffer(Width, Height,
                                               Format, Type,
                                               (void**) &clientBuffer)))
    {
        return clientBuffer;
    }

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
    return IS_SUCCESS(GAL->GAL_GetClientBufferInfo(ClientBuffer,
                                                   Width, Height,
                                                   Stride, Bits)) ? 1 : 0;
}

VDKAPI int VDKLANG
vdkDestroyClientBuffer(
    vdkClientBuffer ClientBuffer
    )
{
    return IS_SUCCESS(GAL->GAL_DestroyClientBuffer(ClientBuffer)) ? 1 : 0;
}
