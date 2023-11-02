#include <VDK/gc_vdk.h>
#include <VG/openvg.h>
#include <stdlib.h>
#include "app.h"
#include "main.h"
#include "util/util.h"
#include "util/st.h"

static void AppSetup(CT_AppRec* pMe)
{
    if (!pMe)
        return;

    STRCPY(pMe->title, CT_RELEASE_TITLE);
    STRCPY(pMe->name, CT_RELEASE_NAME);
    STRCPY(pMe->version, CT_RELEASE_VERSION);
    STRCPY(pMe->date, CT_RELEASE_DATE);

    pMe->verboseLevel = 1;
    pMe->configID = CT_UNSPECIFIED;

    pMe->native_window = 0;
    pMe->native_display = 0;

    pMe->egl_display = EGL_NO_DISPLAY;
    pMe->egl_surface = EGL_NO_SURFACE;
    pMe->egl_context = EGL_NO_CONTEXT;

    Output(pMe, 0, "\n");
    Output(pMe, 0, "%s\n", pMe->title);
    Output(pMe, 0, "version %s (%s)\n\n", pMe->version, pMe->date);
}

void Render(
    vdkEGL * Egl,
    int FrameNumber
    )
{
    CT_AppRec* pMe;

    pMe = (CT_AppRec*)malloc(sizeof(CT_AppRec));
    MEMSET(pMe, 0, sizeof(CT_AppRec));
    AppSetup(pMe);

    if (CT_NO_ERROR != DriverSetup(pMe))
        goto fail;

    LOAD_TIE_BREAK_RULE_MATRIX_FORCE_RECAL();
    /* Begin CTS test */
    Driver(pMe);

fail:
    if (pMe)
        free(pMe);

    EXIT(EXIT_FAILURE);
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
