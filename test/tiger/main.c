#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "VG/openvg.h"
#include "EGL/egl.h"
#include "VDK/gc_vdk.h"
#include "tiger.h"
#ifndef _WIN32
#include <sys/time.h>
#endif
#ifndef min
#define min(x, y) ( ((x) < (y)) ? (x) : (y) )
#endif

vdkEGL    vdk;
int        width, height;
float    x, y, scale, angle;
PS*        tiger = NULL;

float getRandom(float min, float max)
{
    float r = (float) rand() / RAND_MAX;
    return r * (max - min) + min;
}

void center(int flags)
{
    if (flags & 1)
    {
        x = (width - scale * tigerMaxX) / 2.0f;
    }

    if (flags & 2)
    {
        y = (height - scale * tigerMaxY) / 2.0f;
    }
}

void reset(void)
{
    scale = 1.0f;
    angle = 0.0f;
    center(3);
}
static int numFrame = 1000;
static int Frame = 0;
#ifndef _WIN32
long gettick()
{
    long time;
    struct timeval t;
    gettimeofday(&t, NULL);

    time = t.tv_sec * 1000000 + t.tv_usec;

    return time;
}
#endif

int main(int argc, char *argv[])
{
#ifndef _WIN32
    long time ,starttime;
#endif
    int render, run, i;
    int control = 0;
    int flags = 3;
    int animate = 0;
    int verbose = 0;
    double cpu_time_used;

    reset();

    for (i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-w") == 0)
        {
            width = atoi(argv[++i]);
        }

        else if (strcmp(argv[i], "-h") == 0)
        {
            height = atoi(argv[++i]);
        }

        else if (strcmp(argv[i], "-x") == 0)
        {
            x = (float) atof(argv[++i]);
            flags &= ~1;
        }

        else if (strcmp(argv[i], "-y") == 0)
        {
            y = (float) atof(argv[++i]);
            flags &= ~2;
        }

        else if (strcmp(argv[i], "-s") == 0)
        {
            scale = (float) atof(argv[++i]);
        }

        else if (strcmp(argv[i], "-a") == 0)
        {
            angle = (float) atof(argv[++i]);
        }

        else if (strcmp(argv[i], "-v") == 0)
        {
            verbose = 1;
        }

        else
        {
            printf("Usage:\n"
                   "\n"
                   "-w <int>    width of window\n"
                   "-h <int>    height of window\n"
                   "-x <float>  initial x position\n"
                   "-y <float>  initial y position\n"
                   "-s <float>  initial scale\n"
                   "-a <float>  initial angle\n"
                   "-v          verbose\n");
        }
    }

    vdkSetupEGL(-1, -1, width, height, NULL, NULL, VDK_CONTEXT_OPENVG, &vdk);
    vdkSetWindowTitle(vdk.window, "OpenVG Tiger");
    vdkShowWindow(vdk.window);
    vdkGetWindowInfo(vdk.window, NULL, NULL, &width, &height, NULL, NULL);

    tiger = PS_construct(tigerCommands,
                         tigerCommandCount,
                         tigerPoints,
                         tigerPointCount);

    center(flags);
    if (verbose)
    {
#ifndef _WIN32
        starttime = gettick();
#endif
    }
    for (run = render = 1; run && (NULL != tiger);)
    {
        switch (rand() % 4)
        {
        case 0: /* Move x. */
            x += getRandom(-width / 8.0f, width / 8.0f);
            break;
        case 1: /* Move y. */
            y += getRandom(-height / 8.0f, height / 8.0f);
            break;
        case 2: /* Scale. */
            scale *= getRandom(0.5f, 2.0f);
            break;
        case 3: /* Rotate. */
            angle += getRandom(-5.0f, 5.0f);
            break;
        }

        if (render)
        {
            float clearColor[4] = { 1, 1, 1, 1 };
            if (verbose)
            {
                printf("Rendering with scale %.2f, angle %.2f at %.2f,%.2f...",
                       scale,
                       angle, 
                       x, y);
            }
            vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
            vgClear(0, 0, width, height);

            vgLoadIdentity();
            vgTranslate(x, y);
            vgScale(scale, scale);
            vgRotate(angle);
            PS_render(tiger);
            vgFinish();
            assert(vgGetError() == VG_NO_ERROR);
            render = 0;

            vdkSwapEGL(&vdk);
        }
        Frame++;

        if (verbose)
        {
            printf("frame: %d \n", Frame);
#ifndef _WIN32
            time = gettick() - starttime;
            printf("Render   time used %ld us, average time used %lf us , FPS %lf.\n", time, (double)time / Frame, Frame / ((double)time / (1000000)));
#endif
        }
    }
    vdkFinishEGL(&vdk);
    return 0;
}
