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

#include "vg_context.h"


typedef struct
{
    HWND                window;
    HDC                 bufDC;
    HBITMAP             bufDIB;
    unsigned int* tmp;
    int                 tmpWidth;
    int                 tmpHeight;
} OSWindowContext;


static HANDLE mutex = NULL;
static int mutexRefCount = 0;

// Acquired mutex cannot be deinited.
void OSDeinitMutex(void)
{
    EGL_ASSERT(mutex);
    EGL_ASSERT(mutexRefCount == 0);
    BOOL ret = CloseHandle(mutex);
    EGL_ASSERT(ret);
    EGL_UNREF(ret);
}

void OSAcquireMutex(void)
{
    if (!mutex)
    {
        mutex = CreateMutex(NULL, FALSE, NULL);
        mutexRefCount = 0;
    }
    EGL_ASSERT(mutex);
    DWORD ret = WaitForSingleObject(mutex, INFINITE);
    EGL_ASSERT(ret != WAIT_FAILED);
    EGL_UNREF(ret);
    mutexRefCount++;
}

void OSReleaseMutex(void)
{
    EGL_ASSERT(mutex);
    mutexRefCount--;
    EGL_ASSERT(mutexRefCount >= 0);
    BOOL ret = ReleaseMutex(mutex);
    EGL_ASSERT(ret);
    EGL_UNREF(ret);
}

void* OSGetCurrentThreadID(void)
{
    return (void*)GetCurrentThreadId();
}

void OSDestroyDisplay(void* display)
{
    return;
}

VGEGLDisplay* OSGetDisplay(EGLNativeDisplayType display_id)
{
    int bpp;
    VGEGLDisplay* newDisplay = NULL;

    display_id = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

    if (display_id == NULL)
    {
        return NULL;
    }

    bpp = GetDeviceCaps(display_id, BITSPIXEL);

    if ((bpp != 16) && (bpp != 32))
    {
        DeleteDC(display_id);
        return NULL;
    }

    newDisplay = (VGEGLDisplay*)malloc(sizeof(VGEGLDisplay));
    newDisplay->m_id = display_id;

    return newDisplay;
}

void* OSCreateWindowContext(EGLNativeWindowType window)
{
     OSWindowContext *ctx = NULL;

    ctx = malloc(sizeof(OSWindowContext));
    if (ctx)
    {
        ctx->window = (HWND)window;
        HDC winDC = GetDC(ctx->window);
        ctx->bufDC = CreateCompatibleDC(winDC);
        ReleaseDC(ctx->window, winDC);

        if (!ctx->bufDC)
        {
            free(ctx);
            return NULL;
        }

        ctx->bufDIB = NULL;
        ctx->tmp = NULL;
        ctx->tmpWidth = 0;
        ctx->tmpHeight = 0;
    }

    return ctx;
}

void OSDestroyWindowContext(void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;

    if (ctx)
    {
        if (ctx->bufDC)
        {
            SelectObject(ctx->bufDC, NULL);
            DeleteDC(ctx->bufDC);
        }

        if (ctx->bufDIB)
            DeleteObject(ctx->bufDIB);
        free(ctx);
    }
}

VGboolean OSIsWindow(const void* context)
{
    OSWindowContext* ctx = (OSWindowContext*)context;

    if (ctx)
    {
        if (IsWindow(ctx->window))
            return VG_TRUE;
    }

    return VG_FALSE;
}

void OSGetWindowSize(const void* context, int* width, int* height)
{
    OSWindowContext* ctx = (OSWindowContext*)context;

    if (ctx)
    {
        RECT rect;
        GetClientRect(ctx->window, &rect);
        *width = rect.right - rect.left;
        *height = rect.bottom - rect.top;
    }
    else
    {
        *width = 0;
        *height = 0;
    }
}

static VGboolean isBigEndian()
{
    static const VGuint v = 0x12345678u;
    const VGubyte* p = (const VGubyte*)&v;
    VG_ASSERT(*p == (VGubyte)0x12u || *p == (VGubyte)0x78u);
    return (*p == (VGubyte)(0x12)) ? VG_TRUE : VG_FALSE;
}

void OSBlitToWindow(void* context, const Drawable* drawable)
{
    OSWindowContext *ctx = (OSWindowContext *)context;

    if (ctx)
    {
        int w = drawable->m_color->m_width;
        int h = drawable->m_color->m_height;

        if (!ctx->tmp || !ctx->bufDIB || ctx->tmpWidth != w || ctx->tmpHeight != h)
        {
            if (ctx->bufDIB)
                DeleteObject(ctx->bufDIB);
            ctx->tmp = NULL;
            ctx->bufDIB = NULL;

            ctx->tmpWidth = w;
            ctx->tmpHeight = h;

            struct
            {
                BITMAPINFOHEADER    header;
                DWORD                rMask;
                DWORD                gMask;
                DWORD                bMask;
            } bmi;
            bmi.header.biSize            = sizeof(BITMAPINFOHEADER);
            bmi.header.biWidth            = w;
            bmi.header.biHeight            = h;
            bmi.header.biPlanes            = 1;
            bmi.header.biBitCount        = (WORD)32;
            bmi.header.biCompression    = BI_BITFIELDS;
            bmi.rMask = 0x000000ff;
            bmi.gMask = 0x0000ff00;
            bmi.bMask = 0x00ff0000;
            ctx->bufDIB = CreateDIBSection(ctx->bufDC, (BITMAPINFO*)&bmi, DIB_RGB_COLORS, (void**)&ctx->tmp, NULL, 0);
            if (!ctx->bufDIB)
            {
                ctx->tmp = NULL;
                return;
            }
        }

        if (ctx->tmp)
        {
            //NOTE: we assume here that the display is always in sRGB color space
            GdiFlush();
            VGImageFormat f = VG_sXBGR_8888;
            if (isBigEndian())
                f = VG_sRGBX_8888;
            vgReadPixels(ctx->tmp, w*sizeof(unsigned int), f, 0, 0, w, h);

            SelectObject(ctx->bufDC, ctx->bufDIB);
            HDC winDC = GetDC(ctx->window);
            BitBlt(winDC, 0, 0, w, h, ctx->bufDC, 0, 0, SRCCOPY);
            ReleaseDC(ctx->window, winDC);
            SelectObject(ctx->bufDC, NULL);
        }
    }
}

VGuint OSGetPixmapInfo(EGLNativePixmapType pixmap, VGuint *width, VGuint *height, VGImageFormat *format, VGuint *bitsPerPixel, VGuint* stride, VGubyte** bits)
{
    DIBSECTION bitmap;
    int ret;

    ret = GetObject(pixmap, sizeof(bitmap), &bitmap);
    if (!ret)
        return EGL_BAD_NATIVE_PIXMAP;

    if (width!= NULL)
        *width = bitmap.dsBm.bmWidth;

    if (height != NULL)
        *height = bitmap.dsBm.bmHeight;

    if (stride != NULL)
        *stride = bitmap.dsBm.bmWidthBytes;

    if (bitsPerPixel != NULL)
        *bitsPerPixel = bitmap.dsBm.bmBitsPixel;

    if (format != NULL)
    {
        VGuint r, g ,b;
        r = bitmap.dsBitfields[0];
        g = bitmap.dsBitfields[1];
        b = bitmap.dsBitfields[2];

        switch (bitmap.dsBm.bmBitsPixel)
        {
         case 16:
             if ((r == 0x7C00) && (g == 0x03E0) && (b == 0x001F))
             {
                 *format = VG_sRGBA_5551;
             }
             else if ((r == 0x0F00) && (g == 0x00F0) && (b == 0x000F))
             {
                 *format = VG_sRGBA_4444;
             }
             else
             {
                 *format = VG_sRGB_565;
             }
             break;
         case 32:
         case 24:
            {
                *format = VG_sRGBA_8888;
            }
            break;
         default:
            return EGL_BAD_PARAMETER;

        }
    }
    if (bits != NULL)
        *bits = (VGubyte*)bitmap.dsBm.bmBits;

    return EGL_SUCCESS;
}
