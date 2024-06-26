#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "VG/openvg.h"
#include "EGL/egl.h"
#include "VDK/gc_vdk.h"
#include "tiger.h"

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

int main(int argc, char *argv[])
{
    int render, run, i;
    int control = 0;
    int flags = 3;
    int animate = 0;
    int verbose = 0;
    width = 640;
    height = 480;
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

    for (run = render = 1; run && (NULL != tiger);)
    {
        vdkEvent event;
        while (vdkGetEvent(vdk.window, &event))
        {
            switch (event.type)
            {
            case VDK_KEYBOARD:
                if ((event.data.keyboard.scancode == VDK_LCTRL)
                ||  (event.data.keyboard.scancode == VDK_RCTRL)
                )
                {
                    control = event.data.keyboard.pressed;
                    break;
                }
                
                if (event.data.keyboard.pressed)
                {
                    switch (event.data.keyboard.scancode)
                    {
                    case VDK_ESCAPE:
                        run = 0;
                        break;

                    case VDK_EQUAL:
                        scale *= 2.0f;
                        render = 1;
                        break;

                    case VDK_HYPHEN:
                        scale /= 2.0f;
                        render = 1;
                        break;

                    case VDK_LEFT:
                        x     += width / (control ? 16.0f : 4.0f);
                        render = 1;
                        break;

                    case VDK_RIGHT:
                        x     -= width / (control ? 16.0f : 4.0f);
                        render = 1;
                        break;

                    case VDK_UP:
                        y     -= height / (control ? 16.0f : 4.0f);
                        render = 1;
                        break;

                    case VDK_DOWN:
                        y     += height / (control ? 16.0f : 4.0f);
                        render = 1;
                        break;

                    case VDK_SPACE:
                        if (animate)
                        {
                            animate ^= 2;
                        }
                        else
                        {
                            reset();
                            render = 1;
                        }
                        break;

                    case VDK_COMMA:
                        angle += control ? 45.0f : 5.0f;
                        render = 1;
                        break;

                    case VDK_PERIOD:
                        angle -= control ? 45.0f : 5.0f;
                        render = 1;
                        break;

                    case VDK_R:
                        render = 1;
                        break;

                    case VDK_A:
                        animate = !animate;
                        break;

                    case VDK_F:
                        scale  = min(width / tigerMaxX, height / tigerMaxY);
                        angle  = 0.0f;
                        center(3);
                        render = 1;
                        break;

                    default:
                        break;
                    }
                }
                break;

            case VDK_CLOSE:
                run = 0;
                break;

            default:
                break;
            }
        }

        if (animate == 1)
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

            render = 1;
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
            assert(vgGetError() == VG_NO_ERROR);
            render = 0;

            vdkSwapEGL(&vdk);

            if (verbose)
            {
                printf(" Done!\n");
            }
        }
    }

    vdkFinishEGL(&vdk);
    return 0;
}
