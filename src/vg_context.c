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


VGContext* createVgContext(VGContext* shareContext)
{
    VGContext* vgctx = (VGContext*)malloc(sizeof(VGContext));

    if (!vgctx)
    {
        // Out of memory
        return NULL;
    }
    memset(vgctx, 0, sizeof(VGContext));

    // Mode settings
    vgctx->m_matrixMode = VG_MATRIX_PATH_USER_TO_SURFACE;
    vgctx->m_fillRule = VG_EVEN_ODD;
    vgctx->m_imageQuality = VG_IMAGE_QUALITY_FASTER;
    vgctx->m_renderingQuality = VG_RENDERING_QUALITY_BETTER;
    vgctx->m_blendMode = VG_BLEND_SRC_OVER;
    vgctx->m_imageMode = VG_DRAW_IMAGE_NORMAL;

    // Scissor rectangles
    // vgctx->m_scissor();
        
    // Stroke parameters
    vgctx->m_strokeLineWidth = 1.0f;
    vgctx->m_inputStrokeLineWidth = 1.0f;
    vgctx->m_strokeCapStyle = VG_CAP_BUTT;
    vgctx->m_strokeJoinStyle = VG_JOIN_MITER;
    vgctx->m_strokeMiterLimit = 4.0f;
    vgctx->m_inputStrokeMiterLimit = 4.0f;
    // vgctx->m_strokeDashPattern();
    // vgctx->m_inputStrokeDashPattern();
    vgctx->m_strokeDashPhase = 0.0f;
    vgctx->m_inputStrokeDashPhase = 0.0f;
    vgctx->m_strokeDashPhaseReset = VG_FALSE;
    // Edge fill color for vgConvolve and pattern paint
    vgctx->m_tileFillColor.r = 0;
    vgctx->m_tileFillColor.g = 0;
    vgctx->m_tileFillColor.b = 0;
    vgctx->m_tileFillColor.a = 0;
    vgctx->m_tileFillColor.m_format = sRGBA;

    vgctx->m_inputTileFillColor.r = 0;
    vgctx->m_inputTileFillColor.g = 0;
    vgctx->m_inputTileFillColor.b = 0;
    vgctx->m_inputTileFillColor.a = 0;
    vgctx->m_inputTileFillColor.m_format = sRGBA;
    // Color for vgClear
    vgctx->m_clearColor.r = 0;
    vgctx->m_clearColor.g = 0;
    vgctx->m_clearColor.b = 0;
    vgctx->m_clearColor.a = 0;
    vgctx->m_clearColor.m_format = sRGBA;
    vgctx->m_inputClearColor.r = 0;
    vgctx->m_inputClearColor.g = 0;
    vgctx->m_inputClearColor.b = 0;
    vgctx->m_inputClearColor.a = 0;
    vgctx->m_inputClearColor.m_format = sRGBA;

    vgctx->m_glyphOrigin.x = 0.0f;
    vgctx->m_glyphOrigin.y = 0.0f;
    vgctx->m_inputGlyphOrigin.x = 0.0f;
    vgctx->m_inputGlyphOrigin.y = 0.0f;

    vgctx->m_masking = VG_FALSE;
    vgctx->m_scissoring = VG_FALSE;

    vgctx->m_pixelLayout = VG_PIXEL_LAYOUT_UNKNOWN;

    vgctx->m_filterFormatLinear = VG_FALSE;
    vgctx->m_filterFormatPremultiplied = VG_FALSE;
    vgctx->m_filterChannelMask = VG_RED | VG_GREEN | VG_BLUE | VG_ALPHA;

    // Matrices
    vgctx->m_pathUserToSurface.matrix[0][0] = 1.0f;
    vgctx->m_pathUserToSurface.matrix[1][1] = 1.0f;
    vgctx->m_pathUserToSurface.matrix[2][2] = 1.0f;

    vgctx->m_imageUserToSurface.matrix[0][0] = 1.0f;
    vgctx->m_imageUserToSurface.matrix[1][1] = 1.0f;
    vgctx->m_imageUserToSurface.matrix[2][2] = 1.0f;

    vgctx->m_glyphUserToSurface.matrix[0][0] = 1.0f;
    vgctx->m_glyphUserToSurface.matrix[1][1] = 1.0f;
    vgctx->m_glyphUserToSurface.matrix[2][2] = 1.0f;

    vgctx->m_fillPaintToUser.matrix[0][0] = 1.0f;
    vgctx->m_fillPaintToUser.matrix[1][1] = 1.0f;
    vgctx->m_fillPaintToUser.matrix[2][2] = 1.0f;

    vgctx->m_strokePaintToUser.matrix[0][0] = 1.0f;
    vgctx->m_strokePaintToUser.matrix[1][1] = 1.0f;
    vgctx->m_strokePaintToUser.matrix[2][2] = 1.0f;

    vgctx->m_fillPaint = VG_INVALID_HANDLE;
    vgctx->m_strokePaint = VG_INVALID_HANDLE;

    vgctx->m_colorTransform = VG_FALSE;
    vgctx->m_inputColorTransformValues[0] = 1.0f;
    vgctx->m_inputColorTransformValues[1] = 1.0f;
    vgctx->m_inputColorTransformValues[2] = 1.0f;
    vgctx->m_inputColorTransformValues[3] = 1.0f;
    vgctx->m_inputColorTransformValues[4] = 0.0f;
    vgctx->m_inputColorTransformValues[5] = 0.0f;
    vgctx->m_inputColorTransformValues[6] = 0.0f;
    vgctx->m_inputColorTransformValues[7] = 0.0f;
    vgctx->m_colorTransformValues[0] = 1.0f;
    vgctx->m_colorTransformValues[1] = 1.0f;
    vgctx->m_colorTransformValues[2] = 1.0f;
    vgctx->m_colorTransformValues[3] = 1.0f;
    vgctx->m_colorTransformValues[4] = 0.0f;
    vgctx->m_colorTransformValues[5] = 0.0f;
    vgctx->m_colorTransformValues[6] = 0.0f;
    vgctx->m_colorTransformValues[7] = 0.0f;

    vgctx->m_error = VG_NO_ERROR;

    vgctx->m_imageManager = NULL;
    vgctx->m_pathManager = NULL;
    vgctx->m_paintManager = NULL;
    vgctx->m_fontManager = NULL;
    vgctx->m_maskLayerManager = NULL;

    vgctx->m_eglDrawable = NULL;

    if (shareContext)
    {
        vgctx->m_imageManager = shareContext->m_imageManager;
        vgctx->m_pathManager = shareContext->m_pathManager;
        vgctx->m_paintManager = shareContext->m_paintManager;
        vgctx->m_fontManager = shareContext->m_fontManager;
        vgctx->m_maskLayerManager = shareContext->m_maskLayerManager;
    }
    else
    {
        vgctx->m_imageManager = (VGImageManager*)malloc(sizeof(VGImageManager));
        if (!vgctx->m_imageManager)
        {
            // Out of memory
            goto VgContextErr;
        }
        memset(vgctx->m_imageManager, 0, sizeof(VGImageManager));

        vgctx->m_pathManager = (VGPathManager*)malloc(sizeof(VGPathManager));
        if (!vgctx->m_pathManager)
        {
            // Out of memory
            goto VgContextErr;
        }
        memset(vgctx->m_pathManager, 0, sizeof(VGPathManager));

        vgctx->m_paintManager = (VGPaintManager*)malloc(sizeof(VGPaintManager));
        if (!vgctx->m_paintManager)
        {
            // Out of memory
            goto VgContextErr;
        }
        memset(vgctx->m_paintManager, 0, sizeof(VGPaintManager));

        vgctx->m_fontManager = (VGFontManager*)malloc(sizeof(VGFontManager));
        if (!vgctx->m_fontManager)
        {
            // Out of memory
            goto VgContextErr;
        }
        memset(vgctx->m_fontManager, 0, sizeof(VGFontManager));

        vgctx->m_maskLayerManager = (VGMaskLayerManager*)malloc(sizeof(VGMaskLayerManager));
        if (!vgctx->m_maskLayerManager)
        {
            // Out of memory
            goto VgContextErr;
        }
        memset(vgctx->m_maskLayerManager, 0, sizeof(VGMaskLayerManager));

    }
    VG_ASSERT(vgctx->m_imageManager);
    VG_ASSERT(vgctx->m_pathManager);
    VG_ASSERT(vgctx->m_paintManager);
    VG_ASSERT(vgctx->m_fontManager);
    VG_ASSERT(vgctx->m_maskLayerManager);

    vgctx->m_imageManager->m_referenceCount++;
    vgctx->m_pathManager->m_referenceCount++;
    vgctx->m_paintManager->m_referenceCount++;
    vgctx->m_fontManager->m_referenceCount++;
    vgctx->m_maskLayerManager->m_referenceCount++;

    return vgctx;

VgContextErr:
    if (vgctx->m_imageManager) free(vgctx->m_imageManager);
    if (vgctx->m_pathManager) free(vgctx->m_pathManager);
    if (vgctx->m_paintManager) free(vgctx->m_paintManager);
    if (vgctx->m_fontManager) free(vgctx->m_fontManager);
    if (vgctx->m_maskLayerManager) free(vgctx->m_maskLayerManager);

    return NULL;
}

void setDefaultDrawable(VGContext* ctx, Drawable* drawable)
{
    if (ctx->m_eglDrawable)
    {
        ctx->m_eglDrawable->m_referenceCount--; VG_ASSERT(ctx->m_eglDrawable->m_referenceCount >= 0);
        if (!ctx->m_eglDrawable->m_referenceCount)
            destroyDrawable(ctx->m_eglDrawable);
    }
    ctx->m_eglDrawable = drawable;
    if (ctx->m_eglDrawable)
    {
        ctx->m_eglDrawable->m_referenceCount++;
    }
}

VGboolean isValidImage(VGContext* ctx, VGImage image)
{
    VGImageEntry* resptr = NULL;

    if (ctx && ctx->m_imageManager && ctx->m_imageManager->m_resources)
    {
        resptr = ctx->m_imageManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == (Image*)image)
            {
                return VG_TRUE;
            }
            resptr = resptr->next;
        }
    }

    return VG_FALSE;
}

void addImageResource(VGContext* ctx, Image* image)
{
    VGImageEntry* resptr = NULL;

    if (ctx && ctx->m_imageManager)
    {
        resptr = ctx->m_imageManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == image) {
                image->m_referenceCount++;
                return;
            }

            resptr = resptr->next;
        }

        resptr = (VGImageEntry*)malloc(sizeof(VGImageEntry));
        if (!resptr)
        {
            setError(VG_OUT_OF_MEMORY_ERROR);
            return;
        }
        resptr->resource = image;
        resptr->next = ctx->m_imageManager->m_resources;
        ctx->m_imageManager->m_resources = resptr;
        image->m_referenceCount++;
    }
}

void removeImageResource(VGContext* ctx, Image* image)
{
    VGImageEntry *resptr, *prep;

    if (ctx && ctx->m_imageManager)
    {
        resptr = ctx->m_imageManager->m_resources;
        prep = resptr;

        while (resptr)
        {
            if (resptr->resource == image)
            {
                if (resptr == ctx->m_imageManager->m_resources)
                {
                    ctx->m_imageManager->m_resources = resptr->next;
                }
                else
                {
                    prep->next = resptr->next;
                }
                free(resptr);
                break;
            }

            prep = resptr;
            resptr = resptr->next;
        }
    }
}

void destroyImageManager(VGContext* ctx)
{
    VGImageEntry* resptr;

    if (ctx && ctx->m_imageManager)
    {
        resptr = ctx->m_imageManager->m_resources;
        while (resptr)
        {
            ctx->m_imageManager->m_resources = resptr->next;
            free(resptr);
            resptr = ctx->m_imageManager->m_resources;
        }

        free(ctx->m_imageManager);
        ctx->m_imageManager = NULL;
    }
}

VGboolean isValidMaskLayer(VGContext* ctx, VGHandle mask)
{
    VGMaskLayerEntry* resptr = NULL;

    if (ctx && ctx->m_maskLayerManager && ctx->m_maskLayerManager->m_resources)
    {
        resptr = ctx->m_maskLayerManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == (Surface*)mask)
            {
                return VG_TRUE;
            }
            resptr = resptr->next;
        }
    }

    return VG_FALSE;
}

void addMaskLayerResource(VGContext* ctx, Surface* layer)
{
    VGMaskLayerEntry* resptr = NULL;

    if (ctx && ctx->m_maskLayerManager)
    {
        resptr = ctx->m_maskLayerManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == layer) return;
            resptr = resptr->next;
        }

        resptr = (VGMaskLayerEntry*)malloc(sizeof(VGMaskLayerEntry));
        if (!resptr)
        {
            setError(VG_OUT_OF_MEMORY_ERROR);
            return;
        }
        resptr->resource = layer;
        resptr->next = ctx->m_maskLayerManager->m_resources;
        ctx->m_maskLayerManager->m_resources = resptr;
    }
}

void removeMaskLayerResource(VGContext* ctx, Surface* layer)
{
    VGMaskLayerEntry* resptr, * prep;

    if (ctx && ctx->m_maskLayerManager)
    {
        resptr = ctx->m_maskLayerManager->m_resources;
        prep = resptr;

        while (resptr)
        {
            if (resptr->resource == layer)
            {
                if (resptr == ctx->m_maskLayerManager->m_resources)
                {
                    ctx->m_maskLayerManager->m_resources = resptr->next;
                }
                else
                {
                    prep->next = resptr->next;
                }
                free(resptr);
                break;
            }
            prep = resptr;
            resptr = resptr->next;
        }
    }
}

void destroyMaskLayerManager(VGContext* ctx)
{
    VGMaskLayerEntry* resptr;

    if (ctx && ctx->m_maskLayerManager)
    {
        resptr = ctx->m_maskLayerManager->m_resources;
        while (resptr)
        {
            ctx->m_maskLayerManager->m_resources = resptr->next;
            free(resptr);
            resptr = ctx->m_maskLayerManager->m_resources;
        }

        free(ctx->m_maskLayerManager);
        ctx->m_maskLayerManager = NULL;
    }
}

VGboolean isValidPath(VGContext* ctx, VGPath path)
{
    VGPathEntry* resptr = NULL;

    if (ctx && ctx->m_pathManager && ctx->m_pathManager->m_resources)
    {
        resptr = ctx->m_pathManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == (Path*)path)
            {
                return VG_TRUE;
            }
            resptr = resptr->next;
        }
    }

    return VG_FALSE;
}

void addPathResource(VGContext* ctx, Path* path)
{
    VGPathEntry* resptr = NULL;

    if (ctx && ctx->m_pathManager)
    {
        resptr = ctx->m_pathManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == path) return;
            resptr = resptr->next;
        }

        resptr = (VGPathEntry*)malloc(sizeof(VGPathEntry));
        if (!resptr)
        {
            setError(VG_OUT_OF_MEMORY_ERROR);
            return;
        }
        resptr->resource = path;
        resptr->next = ctx->m_pathManager->m_resources;
        ctx->m_pathManager->m_resources = resptr;
    }
}

void removePathResource(VGContext* ctx, Path* path)
{
    VGPathEntry* resptr, * prep;

    if (ctx && ctx->m_pathManager)
    {
        resptr = ctx->m_pathManager->m_resources;
        prep = resptr;

        while (resptr)
        {
            if (resptr->resource == path)
            {
                if (resptr == ctx->m_pathManager->m_resources)
                {
                    ctx->m_pathManager->m_resources = resptr->next;
                }
                else
                {
                    prep->next = resptr->next;
                }
                free(resptr);
                break;
            }
            prep = resptr;
            resptr = resptr->next;
        }
    }
}

void destroyPathManager(VGContext* ctx)
{
    VGPathEntry* resptr;

    if (ctx && ctx->m_pathManager)
    {
        resptr = ctx->m_pathManager->m_resources;
        while (resptr)
        {
            ctx->m_pathManager->m_resources = resptr->next;
            if (resptr->resource)
            {
                freePathImpl((Path*)resptr->resource);
            }
            free(resptr);
            resptr = ctx->m_pathManager->m_resources;
        }

        free(ctx->m_pathManager);
        ctx->m_pathManager = NULL;
    }
}

VGboolean isValidPaint(VGContext* ctx, VGPaint paint)
{
    VGPaintEntry* resptr = NULL;

    if (ctx && ctx->m_paintManager && ctx->m_paintManager->m_resources)
    {
        resptr = ctx->m_paintManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == (Paint*)paint)
            {
                return VG_TRUE;
            }
            resptr = resptr->next;
        }
    }

    return VG_FALSE;
}

void addPaintResource(VGContext* ctx, Paint* paint)
{
    VGPaintEntry* resptr = NULL;

    if (ctx && ctx->m_paintManager)
    {
        resptr = ctx->m_paintManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == paint) return;
            resptr = resptr->next;
        }

        resptr = (VGPaintEntry*)malloc(sizeof(VGPaintEntry));
        if (!resptr)
        {
            setError(VG_OUT_OF_MEMORY_ERROR);
            return;
        }
        resptr->resource = paint;
        resptr->next = ctx->m_paintManager->m_resources;
        ctx->m_paintManager->m_resources = resptr;
        paint->m_referenceCount++;
    }
}

void removePaintResource(VGContext* ctx, Paint* paint)
{
    VGPaintEntry* resptr, * prep;

    if (ctx && ctx->m_paintManager)
    {
        resptr = ctx->m_paintManager->m_resources;
        prep = resptr;

        while (resptr)
        {
            if (resptr->resource == paint)
            {
                if (resptr == ctx->m_paintManager->m_resources)
                {
                    ctx->m_paintManager->m_resources = resptr->next;
                }
                else
                {
                    prep->next = resptr->next;
                }
                free(resptr);
                break;
            }
            prep = resptr;
            resptr = resptr->next;
        }
    }
}

void releasePaint(VGContext* ctx, VGbitfield paintModes)
{
    if (paintModes & VG_FILL_PATH)
    {
        //release previous paint
        Paint* prev = (Paint*)ctx->m_fillPaint;
        if (prev)
        {
            prev->m_referenceCount--;

            if (!prev->m_referenceCount)
                freePaintImpl(prev);
        }
        ctx->m_fillPaint = VG_INVALID_HANDLE;
    }
    if (paintModes & VG_STROKE_PATH)
    {
        //release previous paint
        Paint* prev = (Paint*)ctx->m_strokePaint;
        if (prev)
        {
            prev->m_referenceCount--;
            if (!prev->m_referenceCount)
                freePaintImpl(prev);
        }
        ctx->m_strokePaint = VG_INVALID_HANDLE;
    }
}

void destroyPaintManager(VGContext* ctx)
{
    VGPaintEntry* resptr;

    if (ctx && ctx->m_paintManager)
    {
        resptr = ctx->m_paintManager->m_resources;
        while (resptr)
        {
            ctx->m_paintManager->m_resources = resptr->next;
            if (resptr->resource)
            {
                freePaintImpl((Paint*)resptr->resource);
            }
            free(resptr);
            resptr = ctx->m_paintManager->m_resources;
        }

        free(ctx->m_paintManager);
        ctx->m_paintManager = NULL;
    }
}

VGboolean isValidFont(VGContext* ctx, VGFont font)
{
    VGFontEntry* resptr = NULL;

    if (ctx && ctx->m_fontManager && ctx->m_fontManager->m_resources)
    {
        resptr = ctx->m_fontManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == (Font*)font)
            {
                return VG_TRUE;
            }
            resptr = resptr->next;
        }
    }

    return VG_FALSE;
}

void addFontResource(VGContext* ctx, Font* font)
{
    VGFontEntry* resptr = NULL;

    if (ctx && ctx->m_fontManager)
    {
        resptr = ctx->m_fontManager->m_resources;
        while (resptr)
        {
            if (resptr->resource == font) return;
            resptr = resptr->next;
        }

        resptr = (VGFontEntry*)malloc(sizeof(VGFontEntry));
        if (!resptr)
        {
            setError(VG_OUT_OF_MEMORY_ERROR);
            return;
        }
        resptr->resource = font;
        resptr->next = ctx->m_fontManager->m_resources;
        ctx->m_fontManager->m_resources = resptr;
        font->m_referenceCount++;
    }
}

void removeFontResource(VGContext* ctx, Font* font)
{
    VGFontEntry* resptr, * prep;

    if (ctx && ctx->m_fontManager)
    {
        resptr = ctx->m_fontManager->m_resources;
        prep = resptr;

        while (resptr)
        {
            if (resptr->resource == font)
            {
                if (resptr == ctx->m_fontManager->m_resources)
                {
                    ctx->m_fontManager->m_resources = resptr->next;
                }
                else
                {
                    prep->next = resptr->next;
                }
                free(resptr);
                break;
            }
            prep = resptr;
            resptr = resptr->next;
        }
    }
}

void destroyFontManager(VGContext* ctx)
{
    VGFontEntry* resptr;

    if (ctx && ctx->m_fontManager)
    {
        resptr = ctx->m_fontManager->m_resources;
        while (resptr)
        {
            ctx->m_fontManager->m_resources = resptr->next;
            if (resptr->resource)
            {
                freeFontImpl((Font*)resptr->resource);
            }
            free(resptr);
            resptr = ctx->m_fontManager->m_resources;
        }

        free(ctx->m_fontManager);
        ctx->m_fontManager = NULL;
    }
}

Glyph* findGlyph(Font* font, unsigned int index)
{
    for (int i = 0; i < font->m_glyphSize; i++)
    {
        if (font->m_glyphs[i].m_state != GLYPH_UNINITIALIZED && font->m_glyphs[i].m_index == index)
            return &font->m_glyphs[i];
    }
    return NULL;
}

void destroyVgContext(VGContext* ctx)
{
    releasePaint(ctx, (VG_FILL_PATH | VG_STROKE_PATH));
    setDefaultDrawable(ctx, NULL);

    //decrease the reference count of resource managers
    ctx->m_imageManager->m_referenceCount--;
    if (!ctx->m_imageManager->m_referenceCount)
        destroyImageManager(ctx);

    ctx->m_pathManager->m_referenceCount--;
    if (!ctx->m_pathManager->m_referenceCount)
        destroyPathManager(ctx);

    ctx->m_paintManager->m_referenceCount--;
    if (!ctx->m_paintManager->m_referenceCount)
        destroyPaintManager(ctx);

    ctx->m_fontManager->m_referenceCount--;
    if (!ctx->m_fontManager->m_referenceCount)
        destroyFontManager(ctx);

    ctx->m_maskLayerManager->m_referenceCount--;
    if (!ctx->m_maskLayerManager->m_referenceCount)
        destroyMaskLayerManager(ctx);
    if (ctx)
        free(ctx);
}

void freePathImpl(Path* path) {
    if (path->m_vglPath) {
        vg_lite_clear_path(path->m_vglPath);
        free(path->m_vglPath);
        path->m_vglPath = NULL;
    }

    for (VGint i = 0; i < path->m_numSegments; ++i) {
        free(path->m_data[i]);
    }

    if (path->m_segments) {
        free(path->m_segments);
        path->m_segments = NULL;
    }

    if (path->m_data) {
        free(path->m_data);
        path->m_data = NULL;
    }
    free(path);
}

void freePaintImpl(Paint* paint)
{
    if (paint->m_pattern)
    {
        if (paint->m_pattern->m_vglbuf)
        {
            vg_lite_free(paint->m_pattern->m_vglbuf);
            free(paint->m_pattern->m_vglbuf);
            paint->m_pattern->m_vglbuf = NULL;
            paint->m_pattern->m_data = NULL;
        }
        free(paint->m_pattern);
        paint->m_pattern = NULL;
    }

    free(paint);

}

void freeFontImpl(Font* font)
{
    if (font->m_glyphs)
    {
        for (int i = 0; i < font->m_glyphSize; i++)
        {
            Path* temp = (Path*)font->m_glyphs[i].m_path;
            if (temp)
            {
                temp->m_referenceCount--;
                if (!temp->m_referenceCount) {
                    freePathImpl(temp);
                }
            }
            Image* temp1 = (Image*)font->m_glyphs[i].m_image;
            if (temp1)
            {
                temp1->m_referenceCount--;
                temp1->m_inUse--;
                if (!temp1->m_referenceCount){
                    if (temp1->m_vglbuf)
                    {
                        vg_lite_free((vg_lite_buffer_t*)temp1->m_vglbuf);
                        free((vg_lite_buffer_t*)temp1->m_vglbuf);
                    }
                    free(temp1);
                }
            }
        }
        free((Glyph*)font->m_glyphs);
    }
    free(font);
}


void  setError(int errcode)
{
    VG_GET_CONTEXT(VG_NO_RETVAL);

    if (context->m_error == VG_NO_ERROR)
    {
        context->m_error = errcode;
    }
}
