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
    OpenVG Sample: ClearTarget

    The sample opens a window, sets the title and changes the background
    color in a loop. The sample is implemented based on Vivante Development
    Kit (VDK) to reduce the porting effort between different platforms and
    environments.
*/

#include <VDK/gc_vdk.h>
#include <VG/openvg.h>
#include <stdlib.h>

#define COUNTOF(Array) \
     (sizeof(Array) / sizeof(Array[0]))

typedef VGfloat CLEARCOLOR[4];


/*******************************************************************************
** Main render primitive function.
*/

void Render(
    vdkEGL * Egl,
    int FrameNumber
    )
{
    int x, y, width, height;

    static const CLEARCOLOR clearColors[] =
    {
        { 0.0f, 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 1.0f, 1.0f },
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 0.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f },
    };

    /* Determine clear color index. */
    int index = FrameNumber % COUNTOF(clearColors);

    /* Set the clear color. */
    vgSetfv(VG_CLEAR_COLOR, 4, clearColors[index]);

    /* Get widow rectangle. */
    vdkGetWindowInfo(Egl->window, &x, &y, &width, &height, NULL, NULL);

    /* Clear the frame. */
    vgClear(x, y, width, height);

    /* Display the rendered frame. */
    vdkSwapEGL(Egl);
}


/*******************************************************************************
** All initial OpenVG state settings go in this function.
*/

void InitVGStates(
    void
    )
{
}


/*******************************************************************************
** Main entry.
*/

int main(
    int argc,
    char *argv[]
    )
{
    static vdkEGL egl;
    static int frameNumber;
    static int done = 0;
    static int pause;

    /* Initialize VDK and EGL objects. */
    if (!vdkSetupEGL(
            -1, -1, 320, 240,
            NULL, NULL, VDK_CONTEXT_OPENVG,
            &egl
            ))
    {
        return 0;
    }

    /* Set the title and show the window. */
    vdkSetWindowTitle(egl.window, "Clear Sample");
    vdkShowWindow(egl.window);

    /* Initialize VG state as necessary. */
    InitVGStates();

    /* Main loop */
    while (!done)
    {
        vdkEvent eventInfo;

        /* Get an event. */
        if (vdkGetEvent(egl.window, &eventInfo))
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
            Render(&egl, frameNumber);

            /* Increment the frame number. */
            frameNumber += 1;
        }
    }

    /* Terminate the application on a keystroke. */
    vdkFinishEGL(&egl);

    return 0;
}
