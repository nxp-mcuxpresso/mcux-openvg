/*
    OpenVG Sample: ClearTarget

    The sample opens a window, sets the title and changes the background
    color in a loop. The sample is implemented based on Vivante Development
    Kit (VDK) to reduce the porting effort between different platforms and
    environments.

    Copyright (C) 2009 Vivante Corporation

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
