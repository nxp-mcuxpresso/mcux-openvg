#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "VG/openvg.h"
#include "EGL/egl.h"
#include "VDK/gc_vdk.h"
#include "tiger.h"
#include "sys/time.h"

vdkEGL  vdk;
int     width = 640;
int     height = 480;
float   x, y, scale, angle;
PS*     tiger = NULL;


int main(int argc, char *argv[])
{
    int i = 0;
    int frame = 0;
    float clearColor[4] = { 1, 1, 1, 1 };
    struct timeval time1;
    struct timeval time2;

    vdkSetupEGL(-1, -1, width, height, NULL, NULL, VDK_CONTEXT_OPENVG, &vdk);
    vdkSetWindowTitle(vdk.window, "OpenVG Tiger");
    vdkShowWindow(vdk.window);
    vdkGetWindowInfo(vdk.window, NULL, NULL, &width, &height, NULL, NULL);

    tiger = PS_construct(tigerCommands,
                         tigerCommandCount,
                         tigerPoints,
                         tigerPointCount);

    vgLoadIdentity();
    vgScale(0.6, 0.6);
    vgTranslate(200, 30);

    gettimeofday(&time1, NULL);

    while(1) {
        vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
        vgClear(0, 0, width, height);

        if (frame < 10) {
            vgTranslate(0, -1);
            vgRotate(1);
        }
        else {
            vgTranslate(0, 1);
            vgRotate(-1);
        }

        PS_render(tiger);
        assert(vgGetError() == VG_NO_ERROR);
        vdkSwapEGL(&vdk);
        frame++;
      
        if (frame == 20) {
            gettimeofday(&time2, NULL);
            double times = (time2.tv_sec - time1.tv_sec) * 1000 + (time2.tv_usec - time1.tv_usec) / 1000;
            printf("FPS is: %lf\n", frame * 1000 / times);
            frame = 0;
            time1 = time2;
        }
    }

    vdkFinishEGL(&vdk);
    return 0;
}
