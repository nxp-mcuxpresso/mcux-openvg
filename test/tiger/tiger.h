#ifndef __TIGER_H
#define __TIGER_H

/*------------------------------------------------------------------------
 *
 * OpenVG 1.0.1 Reference Implementation sample code
 * -------------------------------------------------
 *
 * Copyright (c) 2007 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Materials. 
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *//**
 * \file
 * \brief    Header for including the Tiger image data.
 * \note    
 *//*-------------------------------------------------------------------*/

#include <stdio.h>
#include <VG/openvg.h>

#define UNREF(X) ((void)(X))

extern const int tigerCommandCount;
extern const char tigerCommands[];
extern const float tigerMinX;
extern const float tigerMaxX;
extern const float tigerMinY;
extern const float tigerMaxY;
extern const int tigerPointCount;
extern const float tigerPoints[];

typedef struct
{
    VGFillRule        m_fillRule;
    VGPaintMode        m_paintMode;
    VGCapStyle        m_capStyle;
    VGJoinStyle        m_joinStyle;
    float            m_miterLimit;
    float            m_strokeWidth;
    VGPaint            m_fillPaint;
    VGPaint            m_strokePaint;
    VGPath            m_path;
}
PathData;

typedef struct
{
    PathData*        m_paths;
    int                m_numPaths;
}
PS;

PS* PS_construct(const char* commands, int commandCount, 
                 const float* points, int pointCount);
void PS_destruct(PS* ps);
void PS_render(PS* ps);

#endif /* __TIGER_H */
