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
    OpenVG Sample: TSOverflow

    The sample opens a window, sets the title and draws a path on top of itsef
    several times to the point of overflow. This tests how well the overflow
    is handled by the hardware tesselator.

    The sample is implemented based on Vivante Development Kit (VDK) to reduce
    the porting effort between different platforms and environments.
*/

#include <VDK/gc_vdk.h>
#include <VG/openvg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* FPGA system flag. */
#define FPGA_RUN            0

/* The size of the window in pixels. */
#if FPGA_RUN
#    define WND_WIDTH        640
#    define WND_HEIGHT        480
#else
#    define WND_WIDTH        256
#    define WND_HEIGHT        256
#endif

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
    VGPaint        fillPaint;
    VGPaint        strokePaint;
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
    static VGfloat angle = 0.0f;
    VGint x, y, width, height;
    VGint pathWidth, pathHeight;
    VGfloat centerX, centerY;

    /* Get widow rectangle. */
    vdkGetWindowInfo(Test->egl.window, &x, &y, &width, &height, NULL, NULL);

    /* Clear the frame. */
    vgClear(x, y, width, height);
#if 0
    vgFinish();
#endif

    /* Determine path width and height. */
    pathWidth  = WND_WIDTH  - PATH_SPACING_X * 2;
    pathHeight = WND_HEIGHT - PATH_SPACING_Y * 2;

    /* Determine the center of the path. */
    centerX = WND_WIDTH  / 2.0f;
    centerY = WND_HEIGHT / 2.0f;

    /* Rotate the rectangle. */
    vgLoadIdentity();
    vgTranslate( centerX,  centerY);
    vgRotate(angle);
    angle += 10.0f;
    vgTranslate(-centerX, -centerY);

    /* Draw the stroked path. */
#if 1
    vgDrawPath(Test->path, VG_FILL_PATH | VG_STROKE_PATH);
#endif

#if 0
    vgDrawPath(Test->path, VG_FILL_PATH);
#endif

#if 0
    vgDrawPath(Test->path, VG_STROKE_PATH);
#endif

#if 0
    vgFinish();
#endif

    /* Display the rendered frame. */
    vdkSwapEGL(&Test->egl);
}


/*******************************************************************************
** Create a square looping path.
*/

VGPath CreateLoopingSquare(
    int LoopCount
    )
{
    static const unsigned char squareCommands[] =
    {
        VG_MOVE_TO | VG_ABSOLUTE,
        VG_LINE_TO | VG_ABSOLUTE,
        VG_LINE_TO | VG_ABSOLUTE,
        VG_LINE_TO | VG_ABSOLUTE,
        VG_LINE_TO | VG_ABSOLUTE
    };

    static const float squareData[] =
    {
        /* VG_MOVE_TO */
            PATH_SPACING_X,
            PATH_SPACING_Y,

        /* VG_LINE_TO */
            WND_WIDTH  - PATH_SPACING_X,
            PATH_SPACING_Y,

        /* VG_LINE_TO */
            WND_WIDTH  - PATH_SPACING_X,
            WND_HEIGHT - PATH_SPACING_Y,

        /* VG_LINE_TO */
            PATH_SPACING_X,
            WND_HEIGHT - PATH_SPACING_Y,

        /* VG_LINE_TO */
            PATH_SPACING_X,
            PATH_SPACING_Y,
    };

    VGPath path = VG_INVALID_HANDLE;

    unsigned char * pathCommands = NULL;
    unsigned char * pathData     = NULL;

    do
    {
        int i;
        unsigned char * currCommand;
        unsigned char * currData;
        int cmdCount, dataCount;

        /* Determine the number of items. */
        cmdCount  = LoopCount * COUNTOF(squareCommands);
        dataCount = LoopCount * COUNTOF(squareData);

        /* Allocate space for the path commands. */
        pathCommands = malloc(sizeof(squareCommands) * LoopCount);

        if (pathCommands == NULL)
        {
            break;
        }

        /* Allocate space for the path data. */
        pathData = malloc(sizeof(squareData) * LoopCount);

        if (pathData == NULL)
        {
            break;
        }

        /* Set initial pointers. */
        currCommand = pathCommands;
        currData    = pathData;

        /* Fill in the buffers. */
        for (i = 0; i < LoopCount; i += 1)
        {
            memcpy(currCommand, squareCommands, sizeof(squareCommands));
            memcpy(currData,    squareData,     sizeof(squareData));

            currCommand += sizeof(squareCommands);
            currData    += sizeof(squareData);
        }

        path = vgCreatePath(
            VG_PATH_FORMAT_STANDARD,
            VG_PATH_DATATYPE_F,
            1.0f, 0.0f,
            cmdCount,
            dataCount,
            VG_PATH_CAPABILITY_ALL
            );

        if (path == VG_INVALID_HANDLE)
        {
            break;
        }

        vgAppendPathData(
            path,
            cmdCount,
            pathCommands,
            pathData
            );
    }
    while (0);

    if (pathData != NULL)
    {
        free(pathData);
    }

    if (pathCommands != NULL)
    {
        free(pathCommands);
    }

    return path;
}


/*******************************************************************************
** All initial OpenVG state settings go in this function.
*/

int InitVGStates(
    TEST_PTR Test
    )
{
    static const VGfloat fillPaintColor[] =
    {
        0.0f, 1.0f, 0.0f, 1.0f
    };

    static const VGfloat strokePaintColor[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f
    };

    /* Assume failure. */
    int result = 0;

    do
    {
        /*
            Create the path.
        */
        Test->path = CreateLoopingSquare(5);

        if (Test->path == VG_INVALID_HANDLE)
        {
            break;
        }

        /*
            Create fill paint.
        */
        {
            Test->fillPaint = vgCreatePaint();

            if (Test->fillPaint == VG_INVALID_HANDLE)
            {
                break;
            }

            VG_ERROR_BREAK(vgSetParameteri(
                Test->fillPaint,
                VG_PAINT_TYPE,
                VG_PAINT_TYPE_COLOR
                ));

            VG_ERROR_BREAK(vgSetParameterfv(
                Test->fillPaint,
                VG_PAINT_COLOR,
                COUNTOF(fillPaintColor),
                fillPaintColor
                ));

            VG_ERROR_BREAK(vgSetPaint(
                Test->fillPaint,
                VG_FILL_PATH
                ));
        }

        /*
            Create stroke paint.
        */
        {
            Test->strokePaint = vgCreatePaint();

            if (Test->strokePaint == VG_INVALID_HANDLE)
            {
                break;
            }

            VG_ERROR_BREAK(vgSetParameteri(
                Test->strokePaint,
                VG_PAINT_TYPE,
                VG_PAINT_TYPE_COLOR
                ));

            VG_ERROR_BREAK(vgSetParameterfv(
                Test->strokePaint,
                VG_PAINT_COLOR,
                COUNTOF(strokePaintColor),
                strokePaintColor
                ));

            VG_ERROR_BREAK(vgSetPaint(
                Test->strokePaint,
                VG_STROKE_PATH
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

        /* Set stroke parameters. */
        VG_ERROR_BREAK(vgSetf(
            VG_STROKE_LINE_WIDTH,
            4.0f
            ));

        VG_ERROR_BREAK(vgSeti(
            VG_STROKE_CAP_STYLE,
            VG_CAP_ROUND
            ));

        VG_ERROR_BREAK(vgSeti(
            VG_STROKE_JOIN_STYLE,
            VG_JOIN_ROUND
            ));

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
    /* Destroy the fill paint. */
    if (Test->fillPaint != VG_INVALID_HANDLE)
    {
        vgDestroyPaint(Test->fillPaint);
        Test->fillPaint = VG_INVALID_HANDLE;
    }

    /* Destroy the stroke paint. */
    if (Test->strokePaint != VG_INVALID_HANDLE)
    {
        vgDestroyPaint(Test->strokePaint);
        Test->strokePaint = VG_INVALID_HANDLE;
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
    static int done;
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
        vdkSetWindowTitle(test.egl.window, "TS Overflow Sample");
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

#if 0
                    /* Frame count limit. */
                    if (test.frameNumber == 50)
                    {
                        done = 1;
                    }
#endif
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

#if 0

/* COMMAND BUFFER: count=34 (0x22), size=272 bytes @ 00016300. */
unsigned int _TSOverflow_0[] =
{
    0x30010A34, 0xFF7FFFFF,

    0x4000001F, 0xCDCDCDCD,

    0xEEEEEE02, 0x41C80000, 0x41C80000, 0xEEEEEE04, 0x43670000, 0x41C80000,
    0xEEEEEE04, 0x43670000, 0x43670000, 0xEEEEEE04, 0x41C80000, 0x43670000,
    0xEEEEEE04, 0x41C80000, 0x41C80000, 0xEEEEEE02, 0x41C80000, 0x41C80000,
    0xEEEEEE04, 0x43670000, 0x41C80000, 0xEEEEEE04, 0x43670000, 0x43670000,
    0xEEEEEE04, 0x41C80000, 0x43670000, 0xEEEEEE04, 0x41C80000, 0x41C80000,
    0xEEEEEE02, 0x41C80000, 0x41C80000, 0xEEEEEE04, 0x43670000, 0x41C80000,
    0xEEEEEE04, 0x43670000, 0x43670000, 0xEEEEEE04, 0x41C80000, 0x43670000,
    0xEEEEEE04, 0x41C80000, 0x41C80000, 0xEEEEEE02, 0x41C80000, 0x41C80000,
    0xEEEEEE04, 0x43670000, 0x41C80000, 0xEEEEEE04, 0x43670000, 0x43670000,
    0xEEEEEE04, 0x41C80000, 0x43670000, 0xEEEEEE04, 0x41C80000, 0x41C80000,

    0xCDCDCD00, 0xCDCDCDCD,

    0x70000000, 0xCDCDCDCD
};

_ScheduleTasks(610)
  number of tasks scheduled   = 2
  size of event data in bytes = 44
Allocated 3 pages @ 01D6F100
LOCK 01D6F150 for pages @ 01D6F100
  processing tasks for block 8
  first task container for the block added
  copying user tasks over to the kernel
    task ID = 1, size = 8
    task ID = 4, size = 12
    task ID = 4, size = 12
_ScheduleTasks(749): block = 8 interrupt = 3

COMMAND QUEUE DUMP: 2 entries
ENTRY 0
  COMMAND BUFFER: count=60 (0x3C), size=480 bytes @ 0000E040.
    0x301B0A00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x30030A1C, 0x00000000, 0x00000000, 0x00000000,
    0x30030A20, 0x00000000, 0x00000000, 0x00000000, 0x30100A24, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xDEADDEAD, 0x30080A35, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xDEADDEAD,
    0x301B0A3E, 0x00000055, 0x70707074, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x75057545, 0x70007000, 0x70707074, 0x74007000, 0x70007000, 0x70007000, 0x70007000,
    0x00402008, 0x04001000, 0x00800200, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x30190A60, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000D, 0xCDCDCDCD
ENTRY 1
  COMMAND BUFFER: count=27 (0x1B), size=216 bytes @ 0000E2C0.
    0x30010A11, 0x05040000, 0x30010A12, 0x00000100, 0x30010A13, 0x01000100, 0x30010A02, 0xFF000000,
    0x30010A54, 0x00000000, 0x30010A34, 0x564E0764, 0x30010A00, 0x10000001, 0x30010A10, 0x00011100,
    0x40000001, 0x00000000, 0x00000000, 0x01000100, 0x30010A14, 0x05040000, 0x30010A15, 0x00000100,
    0x30010A11, 0x05000000, 0x30010A12, 0x00000400, 0x30010A13, 0x01000100, 0x30010A02, 0x00DA7772,
    0x30010A54, 0x00000000, 0x30010A34, 0x564E0764, 0x30010A00, 0x10000001, 0x30010A10, 0x00000103,
    0x40000001, 0x00000000, 0x00000000, 0x01000100, 0x30010A1B, 0x00000001, 0x10000007, 0x00000000,
    0x20000007, 0x00000000, 0x30010E01, 0x00000703, 0x0000000D, 0xCDCDCDCD

_ProcessInterrupt: triggered=0x00002000
_ProcessInterrupt: interrupt=13
_UpdateStaticCommandBuffer(1913)
_ExecuteDynamicCommandBuffer(2071): executing next buffer @ 0x0000E2C0, data count = 27
_ProcessInterrupt: triggered=0x00002008
_ProcessInterrupt: interrupt=3
_ProcessInterrupt: interrupt=13
_UpdateDynamicCommandBuffer(2045)
_ScheduleTasks(610)
  number of tasks scheduled   = 2
  size of event data in bytes = 44
  processing tasks for block 8
  first task container for the block added
  copying user tasks over to the kernel
    task ID = 1, size = 8
    task ID = 4, size = 12
    task ID = 4, size = 12
_ScheduleTasks(749): block = 8 interrupt = 4
COMMAND QUEUE DUMP: 1 entries
ENTRY 0
  COMMAND BUFFER: count=36 (0x24), size=288 bytes @ 0000E400.

0x0000E400
    0x30010A02, 0xFF00FF00,
    0x30010A1B, 0x00010000,

0x0000E410
    0x30010A30, 0x00026280, 0x30010A31, 0x000A6280, 0x30010A32, 0x000A6680, 0x30010A33, 0x00000800,
    0x30010A01, 0x00000000, 0x30010A35, 0x00026280, 0x30010A36, 0x000A6280, 0x30010A37, 0x000A6680,
    0x30010A38, 0x00000800, 0x30010A39, 0x00000000, 0x30010A3A, 0x01000100, 0x30010A3B, 0x3F800000,
    0x30010A3C, 0x00000000, 0x30060A40, 0x3F800000, 0x00000000, 0x00000000, 0x00000000, 0x3F800000,
    0x00000000, 0xCDCDCDCD,

0x0000E498
    0x80000000, 0xCDCDCDCD,
    0x80000000, 0xCDCDCDCD,
    0x80000000, 0xCDCDCDCD,
    0x80000000, 0xCDCDCDCD,

0x0000E4B8
    0x9000000C, 0x0000E4C0,

0x0000E4C0
    0x30010A54, 0x00000000,
    0x30010A34, 0x577E0777,
    0x30010A00, 0x00000103,
    0x30010A10, 0x00000003,
    0x30010A3D, 0x00000001,

    0x60000022, 0x00016300,

    0x91000000, 0x00000000,

    0x30010A1B, 0x00000001,
    0x10000007, 0x00000000,
    0x20000007, 0x00000000,

    0x30010E01, 0x00000704,
    0x0000000D, 0xCDCDCDCD

ERROR: Counter overflowed at 231.000000,25.250000
INFO: Restarting path at lower quality
_ProcessInterrupt: triggered=0x00000001
_ProcessInterrupt: interrupt=0
_EventHandler_TSOverflow(1086): **** TS OVERFLOW ENCOUNTERED ****
WARNING: Issuing a nested CALL.
_ProcessInterrupt: triggered=0x00000010
_ProcessInterrupt: interrupt=4
_ProcessInterrupt: triggered=0x00002000
_ProcessInterrupt: interrupt=13
_UpdateDynamicCommandBuffer(2045)
#endif
