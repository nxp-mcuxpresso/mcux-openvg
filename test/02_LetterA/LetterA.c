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

/*
    OpenVG Sample: LetterA

    The sample opens a window, sets the title and draws a "rotating" letter A.
    The sample is implemented based on Vivante Development Kit (VDK) to reduce
    the porting effort between different platforms and environments.
*/

#include <VDK/gc_vdk.h>
#include <VG/openvg.h>
#include <stdlib.h>
#include <math.h>

/* The size of the window in pixels. */
#define WND_WIDTH    256
#define WND_HEIGHT    256

/* The value of PI. */
#if !defined(PI)
#    define PI 3.14159265f
#endif

/* The distance from the edges of the window to the imaginary box
   surrounding the path. */
#define PATH_SPACING_X \
    ((int) ((WND_WIDTH) / 10))

#define PATH_SPACING_Y \
    ((int) ((WND_HEIGHT) / 10))

/* The number of items in an array. */
#define COUNTOF(Array) \
    sizeof(Array) / sizeof(Array[0])

/* OpenVG error checking. */
#define VG_ERROR_BREAK(Function) \
    Function; \
    \
    if (vgGetError() != VG_NO_ERROR) \
    { \
        break; \
    }

/* Two-point line equations. */
#define XCOORD(Y, X1, Y1, X2, Y2) \
    (((VGfloat) (((X2) - (X1)) * (((Y) - (Y1))))) / ((Y2) - (Y1)) + (X1))

#define YCOORD(X, X1, Y1, X2, Y2) \
    (((VGfloat) (((Y2) - (Y1)) * (((X) - (X1))))) / ((X2) - (X1)) + (Y1))

typedef VGfloat CLEARCOLOR[4];

/* Test info structure. */
typedef struct _TEST * TEST_PTR;
typedef struct _TEST
{
    vdkEGL        egl;
    VGint        frameNumber;
    VGPaint        paint;
    VGPath        path;
}
TEST;


/*******************************************************************************
** Main render primitive function.
*/

void Render(
    TEST_PTR Test
    )
{
    const VGint   ROTATION_STEPS = 4;
    const VGfloat ROTATION_CAP   = 0.1f;

    VGfloat scale;
    VGfloat offset;
    VGint x    = 0;
    VGint y = 0;
    VGint width = 0;
    VGint height = 0;

    /* Determine the current scale factor in (1:0) range. */
    scale = (VGfloat) fabs(cos(
        (PI * Test->frameNumber) / (ROTATION_STEPS * 2)
        ));

    /* Rescale to avoid zero. */
    scale = ROTATION_CAP + scale * (1.0f - ROTATION_CAP);

    /* Determine the hoizontal offset. */
    offset = 1.0f + (1.0f - scale) * (WND_WIDTH / 2 - 1);

    /* Get widow rectangle. */
    vdkGetWindowInfo(Test->egl.window, &x, &y, &width, &height, NULL, NULL);

    /* Clear the frame. */
    vgClear(x, y, width, height);

    /* Modify the matrix to make a rotation effect. */
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgLoadIdentity();
    vgTranslate(offset, 1.0f);
    vgScale(scale, 1.0f);

    /* Draw the stroked path. */
    vgDrawPath(Test->path, VG_STROKE_PATH);

    /* Display the rendered frame. */
    vdkSwapEGL(&Test->egl);
}


/*******************************************************************************
** All initial OpenVG state settings go in this function.
*/

int InitVGStates(
    TEST_PTR Test
    )
{
    /* Assume failure. */
    int result = 0;

    do
    {
        /*
            Create the path.
        */
        {
            static const unsigned char pathCommands[] =
            {
                VG_MOVE_TO | VG_ABSOLUTE,
                VG_LINE_TO | VG_ABSOLUTE,
                VG_LINE_TO | VG_ABSOLUTE,
                VG_MOVE_TO | VG_ABSOLUTE,
                VG_LINE_TO | VG_ABSOLUTE
            };

            static const float pathData[] =
            {
                /* VG_MOVE_TO */
                    PATH_SPACING_X,
                    PATH_SPACING_Y,

                /* VG_LINE_TO */
                    WND_WIDTH  / 2,
                    WND_HEIGHT - PATH_SPACING_Y,

                /* VG_LINE_TO */
                    WND_WIDTH - PATH_SPACING_X,
                    PATH_SPACING_Y,

                /* VG_MOVE_TO */
                    XCOORD
                    (
                        PATH_SPACING_Y + (WND_WIDTH - 2 * PATH_SPACING_Y) / 3,
                            PATH_SPACING_X,
                            PATH_SPACING_Y,
                            WND_WIDTH  / 2,
                            WND_HEIGHT - PATH_SPACING_Y
                    ),
                    PATH_SPACING_Y + (WND_WIDTH - 2 * PATH_SPACING_Y) / 3,

                /* VG_LINE_TO */
                    XCOORD
                    (
                        PATH_SPACING_Y + (WND_WIDTH - 2 * PATH_SPACING_Y) / 3,
                            WND_WIDTH  / 2,
                            WND_HEIGHT - PATH_SPACING_Y,
                            WND_WIDTH  - PATH_SPACING_X,
                            PATH_SPACING_Y
                    ),
                    PATH_SPACING_Y + (WND_WIDTH - 2 * PATH_SPACING_Y) / 3
            };

            Test->path = vgCreatePath(
                VG_PATH_FORMAT_STANDARD,
                VG_PATH_DATATYPE_F,
                1.0f, 0.0f,
                COUNTOF(pathCommands),
                COUNTOF(pathData),
                VG_PATH_CAPABILITY_ALL
                );

            if (Test->path == VG_INVALID_HANDLE)
            {
                break;
            }

            VG_ERROR_BREAK(vgAppendPathData(
                Test->path,
                COUNTOF(pathCommands),
                pathCommands,
                pathData
                ));
        }

        /*
            Create stroke paint.
        */
        {
            static const VGfloat paintColor[] =
            {
                1.0f, 1.0f, 1.0f, 1.0f
            };

            Test->paint = vgCreatePaint();

            if (Test->paint == VG_INVALID_HANDLE)
            {
                break;
            }

            VG_ERROR_BREAK(vgSetParameteri(
                Test->paint,
                VG_PAINT_TYPE,
                VG_PAINT_TYPE_COLOR
                ));

            VG_ERROR_BREAK(vgSetParameterfv(
                Test->paint,
                VG_PAINT_COLOR,
                COUNTOF(paintColor),
                paintColor
                ));

            VG_ERROR_BREAK(vgSetPaint(
                Test->paint,
                VG_STROKE_PATH
                ));

            VG_ERROR_BREAK(vgSetf(
                VG_STROKE_LINE_WIDTH,
                5.0f
                ));

            VG_ERROR_BREAK(vgSeti(
                VG_STROKE_CAP_STYLE,
                VG_CAP_ROUND
                ));

            VG_ERROR_BREAK(vgSeti(
                VG_STROKE_JOIN_STYLE,
                VG_JOIN_ROUND
                ));
        }

        /* Set clear color. */
        {
            static const VGfloat clearColor[] =
            {
                114.0f / 255.0f,
                119.0f / 255.0f,
                218.0f / 255.0f,
                0.0f
            };

            VG_ERROR_BREAK(vgSetfv(
                VG_CLEAR_COLOR,
                COUNTOF(clearColor),
                clearColor
                ));
        }

        /* Success. */
        result = 1;
    }
    while (VG_FALSE);

    /* Return result. */
    return result;
}


/*******************************************************************************
** Free all allocated resources.
*/

void FreeResources(
    TEST_PTR Test
    )
{
    /* Destroy the paint. */
    if (Test->paint != VG_INVALID_HANDLE)
    {
        vgDestroyPaint(Test->paint);
        Test->paint = VG_INVALID_HANDLE;
    }

    /* Destroy the path. */
    if (Test->path != VG_INVALID_HANDLE)
    {
        vgDestroyPath(Test->path);
        Test->path = VG_INVALID_HANDLE;
    }
}


/*******************************************************************************
** Main entry.
*/

int main(
    int argc,
    char *argv[]
    )
{
    int result;
    static TEST test;
    static int done = 0;
    static int pause;

    /* Initialize VDK and EGL objects. */
    result = vdkSetupEGL(
        -1, -1, WND_WIDTH, WND_HEIGHT,
        NULL, NULL, VDK_CONTEXT_OPENVG,
        &test.egl
        );

    if (result != 0)
    {
        /* Set the title and show the window. */
        vdkSetWindowTitle(test.egl.window, "Letter A Sample");
        vdkShowWindow(test.egl.window);

        /* Initialize VG state as necessary. */
        result = InitVGStates(&test);

        if (result != 0)
        {
            /* Main loop */
            while (!done)
            {
                vdkEvent eventInfo;

                /* Get an event. */
                if (vdkGetEvent(test.egl.window, &eventInfo))
                {
                    /* Test for keyboard event. */
                    if ((eventInfo.type == VDK_KEYBOARD) &&
                        eventInfo.data.keyboard.pressed)
                    {
                        /* Test for key. */
                        switch (eventInfo.data.keyboard.scancode)
                        {
                        case VDK_SPACE:
                            /* Use SPACE to pause. */
                            pause = !pause;
                            break;

                        case VDK_ESCAPE:
                            /* Use ESCAPE to quit. */
                            done = 1;
                            break;

                        default:
                            break;
                        }
                    }

                    /* Test for Close event. */
                    else if (eventInfo.type == VDK_CLOSE)
                    {
                        done = 1;
                    }
                }
                else if (!pause)
                {
                    /* No events, render the next frame. */
                    Render(&test);

                    /* Increment the frame number. */
                    test.frameNumber += 1;
                }
            }
        }

        /* Free allocated reosurces. */
        FreeResources(&test);

        /* Terminate the application on a keystroke. */
        vdkFinishEGL(&test.egl);
    }

    return 0;
}
