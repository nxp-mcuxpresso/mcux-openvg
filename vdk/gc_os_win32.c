
#include <windows.h>

/*******************************************************************************
 * Events. *********************************************************************
 */

#define gcmHEADER()
#define gcmFOOTER()
#define gcmHEADER_ARG(Text, ...)
#define gcmFOOTER_ARG(Text, ...)
#define gcmFOOTER_NO()

typedef enum _halEventType {
    /* Keyboard event. */
    HAL_KEYBOARD,

    /* Mouse move event. */
    HAL_POINTER,

    /* Mouse button event. */
    HAL_BUTTON,

    /* Application close event. */
    HAL_CLOSE,

    /* Application window has been updated. */
    HAL_WINDOW_UPDATE
} halEventType;

/* Scancodes for keyboard. */
typedef enum _halKeys {
    HAL_UNKNOWN = -1,

    HAL_BACKSPACE = 0x08,
    HAL_TAB,
    HAL_ENTER = 0x0D,
    HAL_ESCAPE = 0x1B,

    HAL_SPACE = 0x20,
    HAL_SINGLEQUOTE = 0x27,
    HAL_PAD_ASTERISK = 0x2A,
    HAL_COMMA = 0x2C,
    HAL_HYPHEN,
    HAL_PERIOD,
    HAL_SLASH,
    HAL_0,
    HAL_1,
    HAL_2,
    HAL_3,
    HAL_4,
    HAL_5,
    HAL_6,
    HAL_7,
    HAL_8,
    HAL_9,
    HAL_SEMICOLON = 0x3B,
    HAL_EQUAL = 0x3D,
    HAL_A = 0x41,
    HAL_B,
    HAL_C,
    HAL_D,
    HAL_E,
    HAL_F,
    HAL_G,
    HAL_H,
    HAL_I,
    HAL_J,
    HAL_K,
    HAL_L,
    HAL_M,
    HAL_N,
    HAL_O,
    HAL_P,
    HAL_Q,
    HAL_R,
    HAL_S,
    HAL_T,
    HAL_U,
    HAL_V,
    HAL_W,
    HAL_X,
    HAL_Y,
    HAL_Z,
    HAL_LBRACKET,
    HAL_BACKSLASH,
    HAL_RBRACKET,
    HAL_BACKQUOTE = 0x60,

    HAL_F1 = 0x80,
    HAL_F2,
    HAL_F3,
    HAL_F4,
    HAL_F5,
    HAL_F6,
    HAL_F7,
    HAL_F8,
    HAL_F9,
    HAL_F10,
    HAL_F11,
    HAL_F12,

    HAL_LCTRL,
    HAL_RCTRL,
    HAL_LSHIFT,
    HAL_RSHIFT,
    HAL_LALT,
    HAL_RALT,
    HAL_CAPSLOCK,
    HAL_NUMLOCK,
    HAL_SCROLLLOCK,
    HAL_PAD_0,
    HAL_PAD_1,
    HAL_PAD_2,
    HAL_PAD_3,
    HAL_PAD_4,
    HAL_PAD_5,
    HAL_PAD_6,
    HAL_PAD_7,
    HAL_PAD_8,
    HAL_PAD_9,
    HAL_PAD_HYPHEN,
    HAL_PAD_PLUS,
    HAL_PAD_SLASH,
    HAL_PAD_PERIOD,
    HAL_PAD_ENTER,
    HAL_SYSRQ,
    HAL_PRNTSCRN,
    HAL_BREAK,
    HAL_UP,
    HAL_LEFT,
    HAL_RIGHT,
    HAL_DOWN,
    HAL_HOME,
    HAL_END,
    HAL_PGUP,
    HAL_PGDN,
    HAL_INSERT,
    HAL_DELETE,
    HAL_LWINDOW,
    HAL_RWINDOW,
    HAL_MENU,
    HAL_POWER,
    HAL_SLEEP,
    HAL_WAKE
} halKeys;

/* Structure that defined keyboard mapping. */
typedef struct _halKeyMap {
    /* Normal key. */
    halKeys normal;

    /* Extended key. */
    halKeys extended;
} halKeyMap;

typedef HDC     HALNativeDisplayType;
typedef HWND    HALNativeWindowType;
typedef HBITMAP HALNativePixmapType;

typedef struct __BITFIELDINFO {
    BITMAPINFO  bmi;
    RGBQUAD     bmiColors[2];
} BITFIELDINFO;

/*******************************************************************************
** Default keyboard map. *******************************************************
*/
static halKeyMap keys[] =
{
    /* 00 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 01 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 02 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 03 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 04 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 05 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 06 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 07 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 08 */ { HAL_BACKSPACE,       HAL_UNKNOWN     },
    /* 09 */ { HAL_TAB,             HAL_UNKNOWN     },
    /* 0A */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 0B */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 0C */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 0D */ { HAL_ENTER,           HAL_UNKNOWN     },
    /* 0E */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 0F */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 10 */ { HAL_LSHIFT,          HAL_NUMLOCK     },
    /* 11 */ { HAL_LCTRL,           HAL_SCROLLLOCK  },
    /* 12 */ { HAL_LALT,            HAL_UNKNOWN     },
    /* 13 */ { HAL_BREAK,           HAL_UNKNOWN     },
    /* 14 */ { HAL_CAPSLOCK,        HAL_UNKNOWN     },
    /* 15 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 16 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 17 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 18 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 19 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 1A */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 1B */ { HAL_ESCAPE,          HAL_UNKNOWN     },
    /* 1C */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 1D */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 1E */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 1F */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 20 */ { HAL_SPACE,           HAL_LSHIFT      },
    /* 21 */ { HAL_PGUP,            HAL_RSHIFT      },
    /* 22 */ { HAL_PGDN,            HAL_LCTRL       },
    /* 23 */ { HAL_END,             HAL_RCTRL       },
    /* 24 */ { HAL_HOME,            HAL_LALT        },
    /* 25 */ { HAL_LEFT,            HAL_RALT        },
    /* 26 */ { HAL_UP,              HAL_UNKNOWN     },
    /* 27 */ { HAL_RIGHT,           HAL_UNKNOWN     },
    /* 28 */ { HAL_DOWN,            HAL_UNKNOWN     },
    /* 29 */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 2A */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 2B */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 2C */ { HAL_PRNTSCRN,        HAL_UNKNOWN     },
    /* 2D */ { HAL_INSERT,          HAL_UNKNOWN     },
    /* 2E */ { HAL_DELETE,          HAL_UNKNOWN     },
    /* 2F */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 30 */ { HAL_0,               HAL_UNKNOWN     },
    /* 31 */ { HAL_1,               HAL_UNKNOWN     },
    /* 32 */ { HAL_2,               HAL_UNKNOWN     },
    /* 33 */ { HAL_3,               HAL_UNKNOWN     },
    /* 34 */ { HAL_4,               HAL_UNKNOWN     },
    /* 35 */ { HAL_5,               HAL_UNKNOWN     },
    /* 36 */ { HAL_6,               HAL_UNKNOWN     },
    /* 37 */ { HAL_7,               HAL_UNKNOWN     },
    /* 38 */ { HAL_8,               HAL_UNKNOWN     },
    /* 39 */ { HAL_9,               HAL_UNKNOWN     },
    /* 3A */ { HAL_UNKNOWN,         HAL_SEMICOLON   },
    /* 3B */ { HAL_UNKNOWN,         HAL_EQUAL       },
    /* 3C */ { HAL_UNKNOWN,         HAL_COMMA       },
    /* 3D */ { HAL_UNKNOWN,         HAL_HYPHEN      },
    /* 3E */ { HAL_UNKNOWN,         HAL_PERIOD      },
    /* 3F */ { HAL_UNKNOWN,         HAL_SLASH       },
    /* 40 */ { HAL_UNKNOWN,         HAL_BACKQUOTE   },
    /* 41 */ { HAL_A,               HAL_UNKNOWN     },
    /* 42 */ { HAL_B,               HAL_UNKNOWN     },
    /* 43 */ { HAL_C,               HAL_UNKNOWN     },
    /* 44 */ { HAL_D,               HAL_UNKNOWN     },
    /* 45 */ { HAL_E,               HAL_UNKNOWN     },
    /* 46 */ { HAL_F,               HAL_UNKNOWN     },
    /* 47 */ { HAL_G,               HAL_UNKNOWN     },
    /* 48 */ { HAL_H,               HAL_UNKNOWN     },
    /* 49 */ { HAL_I,               HAL_UNKNOWN     },
    /* 4A */ { HAL_J,               HAL_UNKNOWN     },
    /* 4B */ { HAL_K,               HAL_UNKNOWN     },
    /* 4C */ { HAL_L,               HAL_UNKNOWN     },
    /* 4D */ { HAL_M,               HAL_UNKNOWN     },
    /* 4E */ { HAL_N,               HAL_UNKNOWN     },
    /* 4F */ { HAL_O,               HAL_UNKNOWN     },
    /* 50 */ { HAL_P,               HAL_UNKNOWN     },
    /* 51 */ { HAL_Q,               HAL_UNKNOWN     },
    /* 52 */ { HAL_R,               HAL_UNKNOWN     },
    /* 53 */ { HAL_S,               HAL_UNKNOWN     },
    /* 54 */ { HAL_T,               HAL_UNKNOWN     },
    /* 55 */ { HAL_U,               HAL_UNKNOWN     },
    /* 56 */ { HAL_V,               HAL_UNKNOWN     },
    /* 57 */ { HAL_W,               HAL_UNKNOWN     },
    /* 58 */ { HAL_X,               HAL_UNKNOWN     },
    /* 59 */ { HAL_Y,               HAL_UNKNOWN     },
    /* 5A */ { HAL_Z,               HAL_UNKNOWN     },
    /* 5B */ { HAL_LWINDOW,         HAL_LBRACKET    },
    /* 5C */ { HAL_RWINDOW,         HAL_BACKSLASH   },
    /* 5D */ { HAL_MENU,            HAL_RBRACKET    },
    /* 5E */ { HAL_UNKNOWN,         HAL_SINGLEQUOTE },
    /* 5F */ { HAL_SLEEP,           HAL_UNKNOWN     },
    /* 60 */ { HAL_PAD_0,           HAL_UNKNOWN     },
    /* 61 */ { HAL_PAD_1,           HAL_UNKNOWN     },
    /* 62 */ { HAL_PAD_2,           HAL_UNKNOWN     },
    /* 63 */ { HAL_PAD_3,           HAL_UNKNOWN     },
    /* 64 */ { HAL_PAD_4,           HAL_UNKNOWN     },
    /* 65 */ { HAL_PAD_5,           HAL_UNKNOWN     },
    /* 66 */ { HAL_PAD_6,           HAL_UNKNOWN     },
    /* 67 */ { HAL_PAD_7,           HAL_UNKNOWN     },
    /* 68 */ { HAL_PAD_8,           HAL_UNKNOWN     },
    /* 69 */ { HAL_PAD_9,           HAL_UNKNOWN     },
    /* 6A */ { HAL_PAD_ASTERISK,    HAL_UNKNOWN     },
    /* 6B */ { HAL_PAD_PLUS,        HAL_UNKNOWN     },
    /* 6C */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 6D */ { HAL_PAD_HYPHEN,      HAL_UNKNOWN     },
    /* 6E */ { HAL_PAD_PERIOD,      HAL_UNKNOWN     },
    /* 6F */ { HAL_PAD_SLASH,       HAL_UNKNOWN     },
    /* 70 */ { HAL_F1,              HAL_UNKNOWN     },
    /* 71 */ { HAL_F2,              HAL_UNKNOWN     },
    /* 72 */ { HAL_F3,              HAL_UNKNOWN     },
    /* 73 */ { HAL_F4,              HAL_UNKNOWN     },
    /* 74 */ { HAL_F5,              HAL_UNKNOWN     },
    /* 75 */ { HAL_F6,              HAL_UNKNOWN     },
    /* 76 */ { HAL_F7,              HAL_UNKNOWN     },
    /* 77 */ { HAL_F8,              HAL_UNKNOWN     },
    /* 78 */ { HAL_F9,              HAL_UNKNOWN     },
    /* 79 */ { HAL_F10,             HAL_UNKNOWN     },
    /* 7A */ { HAL_F11,             HAL_UNKNOWN     },
    /* 7B */ { HAL_F12,             HAL_UNKNOWN     },
    /* 7C */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 7D */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 7E */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
    /* 7F */ { HAL_UNKNOWN,         HAL_UNKNOWN     },
};


/* Event structure. */
typedef struct _halEvent {
    /* Event type. */
    halEventType type;

    /* Event data union. */
    union _halEventData {
        /* Event data for keyboard. */
        struct _halKeyboard {
            /* Scancode. */
            halKeys scancode;

            /* ASCII characte of the key pressed. */
            char    key;

            /* Flag whether the key was pressed (1) or released (0). */
            char    pressed;
        } keyboard;

        /* Event data for pointer. */
        struct _halPointer {
            /* Current pointer coordinate. */
            int     x;
            int     y;
        } pointer;

        /* Event data for mouse buttons. */
        struct _halButton {
            /* Left button state. */
            int     left;

            /* Middle button state. */
            int     middle;

            /* Right button state. */
            int     right;

            /* Current pointer coordinate. */
            int     x;
            int     y;
        } button;
    } data;
} halEvent;


#ifndef WS_POPUPWINDOW
#   define WS_POPUPWINDOW (WS_POPUP | WS_BORDER | WS_SYSMENU)
#endif

/*
    _WindowProc

    Callback fuction that processes messages send to a window.

    PARAMETERS:

        HWND Window
            Handle of the window that received a message.

        UINT Message
            Specifies the message.

        WPARAM ParameterW
            Specifies additional message information.

        LPARAM ParameterL
            Specifies additional message information.

    RETURNS:

        LRESULT
            The return value depends on the message.
*/
LRESULT CALLBACK
_WindowProc(
    HWND Window,
    UINT Message,
    WPARAM ParameterW,
    LPARAM ParameterL
    )
{
    /* We do nothing here - just return the default method for the message. */
    return DefWindowProc(Window, Message, ParameterW, ParameterL);
}

/*
    _Error

    Function that display the last known system error in a message box.

    PARAMETERS:

        LPCTSTR Title
            Pointer to the title of the message box.

    RETURNS:

        Nothing.
*/
static void
_Error(
    LPCTSTR Title
    )
{
    LPTSTR buffer;

    /* Get the last known system error and return a formatted message. */
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR) &buffer,
                  0,
                  NULL);

    /* Pop-up a message box with the error. */
    MessageBox(NULL, buffer, Title, MB_ICONSTOP | MB_OK);

    /* Free the formatted message. */
    LocalFree(buffer);
}

/* Register class. */
static ATOM        regClass;


/*******************************************************************************
** Display. ********************************************************************
*/


int
gcoOS_GetDisplay(
    OUT HALNativeDisplayType * Display,
    IN void* Context
    )
{
    int bpp;
    int status = 0;
    gcmHEADER();
    do
    {

        /* Create the device context. */
        *Display = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

        if (*Display == NULL)
        {
            /* Break if the device context could not be created. */
            _Error(TEXT("CreateDC"));
            status = -16;
            break;
        }

        /* Get the display size and color depth. */
        bpp    = GetDeviceCaps(*Display, BITSPIXEL);
        if((bpp != 16) && (bpp != 32))
        {
            _Error(TEXT("Unsupported color depth."));
            status = -13;
            break;
        }

        /* Return the pointer to the display data structure. */
        gcmFOOTER_ARG("*Display=0x%x", *Display);
        return status;
    }
    while (0);

    /* Roll back on error. */
    if (*Display != NULL)
    {
        DeleteDC(*Display);
    }

    /* Error. */
    gcmFOOTER();
    return status;
}

int
gcoOS_GetDisplayByIndex(
    IN int DisplayIndex,
    OUT HALNativeDisplayType * Display,
    IN void* Context
    )
{
    return gcoOS_GetDisplay(Display, Context);
}

int
gcoOS_GetDisplayInfo(
    IN HALNativeDisplayType Display,
    OUT int * Width,
    OUT int * Height,
    OUT size_t * Physical,
    OUT int * Stride,
    OUT int * BitsPerPixel
    )
{
    int status = 0;
    gcmHEADER_ARG("Display=0x%x", Display);
    if (Display == NULL)
    {
        /* Display is not a valid display data structure pointer. */
        status = -1;
        gcmFOOTER();
        return status;
    }

    if (Width != NULL)
    {
        /* Return the width of the display. */
        *Width = GetDeviceCaps(Display, HORZRES);
    }

    if (Height != NULL)
    {
        /* Return the height of the display. */
        *Height = GetDeviceCaps(Display, VERTRES);
    }

    if (Physical != NULL)
    {
        /* The physical address of the display is not known in the Windows
        ** environment. */
        *Physical = ~0U;
    }

    if (Stride != NULL)
    {
        /* The stride of the display is not known in the Windows environment. */
        *Stride = -1;
    }

    if (BitsPerPixel != NULL)
    {
        /* Return the color depth of the display. */
        *BitsPerPixel = GetDeviceCaps(Display, BITSPIXEL);
    }

    /* Success. */
    gcmFOOTER_NO();
    return status;
}

int
gcoOS_DestroyDisplay(
    IN HALNativeDisplayType Display
    )
{
    /* Only process if we have a valid pointer. */
    if (Display != NULL)
    {
        /* Delete the device context. */
        DeleteDC(Display);
    }
    return 0;
}

/*******************************************************************************
** Windows. ********************************************************************
*/

int
gcoOS_CreateWindow(
    IN HALNativeDisplayType Display,
    IN int X,
    IN int Y,
    IN int Width,
    IN int Height,
    OUT HALNativeWindowType * Window
    )
{
    int displayWidth, displayHeight;
    int status = 0;
    gcmHEADER_ARG("Display=0x%x X=%d Y=%d Width=%d Height=%d", Display, X, Y, Width, Height);
    /* Test if we have a valid display data structure pointer. */
    if (Display == NULL)
    {
        status = -1;
        gcmFOOTER();
        return status;
    }
    displayWidth = GetDeviceCaps(Display, HORZRES);
    displayHeight = GetDeviceCaps(Display, VERTRES);

    /* Test for zero width. */
    if (Width == 0)
    {
        /* Use display width instead. */
        Width = displayWidth;
    }
    else
    {
        /* Clamp width to display width. */
        Width = min(Width, displayWidth);
    }

    /* Test for zero height. */
    if (Height == 0)
    {
        /* Use display height instead. */
        Height = displayHeight;
    }
    else
    {
        /* Clamp height to display height. */
        Height = min(Height, displayHeight);
    }

    /* Test for auto-center X coordinate. */
    if (X == -1)
    {
        /* Center the window horizontally. */
        X = (displayWidth - Width) / 2;
    }

    /* Test for auto-center Y coordinate. */
    if (Y == -1)
    {
        /* Center the window vertically. */
        Y = (displayHeight - Height) / 2;
    }

    /* Clamp coordinates to display. */
    if (X < 0) X = 0;
    if (Y < 0) Y = 0;
    if (X + Width  > displayWidth)  Width  = displayWidth  - X;
    if (Y + Height > displayHeight) Height = displayHeight - Y;

    do
    {
        /* Window rectangle. */
        RECT rect;

        /* Window style. */
        UINT style = WS_POPUPWINDOW | WS_CAPTION;
        UINT extra = 0;

        SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, FALSE);
        if (Y + Height > rect.bottom)
        {
            /*extra = WS_EX_TOPMOST;*/
        }

        /* Set the window rectangle. */
        rect.left   = X;
        rect.top    = Y;
        rect.right  = X + Width;
        rect.bottom = Y + Height;

        /* Adjust the window rectangle for the style. */
        AdjustWindowRectEx(&rect, style, FALSE, extra);

        /* Create the window. */
        *Window = CreateWindowEx(extra,
                                TEXT("halClass"),
                                TEXT("vdkApp"),
                                style,
                                rect.left,
                                rect.top,
                                rect.right - rect.left,
                                rect.bottom - rect.top,
                                NULL,
                                NULL,
                                NULL,
                                NULL);

        if (*Window == NULL)
        {
            /* Break on bad window. */
            _Error(TEXT("CreateWindow"));
            break;
        }

        /* Return pointer to the window data structure. */
        gcmFOOTER_ARG("*Window=0x%x", *Window);
        return status;
    }
    while (0);

    if(*Window != NULL)
        DestroyWindow(*Window);
    /* Error. */
    status = -16;
    gcmFOOTER();
    return status;
}

int
gcoOS_GetWindowInfo(
    IN HALNativeDisplayType Display,
    IN HALNativeWindowType Window,
    OUT int * X,
    OUT int * Y,
    OUT int * Width,
    OUT int * Height,
    OUT int * BitsPerPixel,
    OUT unsigned int * Offset
    )
{
    RECT rect;
    int status = 0;
    gcmHEADER_ARG("Display=0x%x Window=0x%x", Display, Window);
    if (Window == NULL)
    {
        /* Window is not a valid window data structure pointer. */
        status = -1;
        gcmFOOTER();
        return status;
    }

    /* Get client rectangle of the window. */
    if (IsWindow((HWND) Window))
    {
        GetClientRect((HWND) Window, &rect);
    }
    else
    {
        status = -1;
        gcmFOOTER();
        return status;
    }

    if (X != NULL)
    {
        /* Return the x coordinate. */
        *X = rect.left;
    }

    if (Y != NULL)
    {
        /* Return the y coordinate. */
        *Y = rect.top;
    }

    if (Width != NULL)
    {
        /* Return the window width. */
        *Width = rect.right - rect.left;
    }

    if (Height != NULL)
    {
        /* Return the window height. */
        *Height = rect.bottom - rect.top;
    }

    if (BitsPerPixel != NULL)
    {
        /* Return the window color depth. */
        if (IsWindow((HWND) Window))
        {
            HDC dc = GetDC((HWND) Window);
            *BitsPerPixel = GetDeviceCaps(dc, BITSPIXEL);
            ReleaseDC((HWND) Window, dc);
        }
        else
        {
            status = -1;
            gcmFOOTER();
            return status;
        }
    }

    if (Offset != NULL)
    {
        /* Offset is not known in the Windows environment. */
        *Offset = ~0U;
    }

    /* Success. */
    gcmFOOTER_NO();
    return status;
}

int
gcoOS_DestroyWindow(
    IN HALNativeDisplayType Display,
    IN HALNativeWindowType Window
    )
{
    /* Only process if we have a valid pointer */
    if (Window != NULL)
    {
        /* Destroy the window. */
        DestroyWindow(Window);
    }
    return 0;
}

/*******************************************************************************
** Pixmaps. ********************************************************************
*/

int
gcoOS_CreatePixmap(
    IN HALNativeDisplayType Display,
    IN int Width,
    IN int Height,
    IN int BitsPerPixel,
    OUT HALNativePixmapType * Pixmap
    )
{
    int status = 0;
    gcmHEADER_ARG("Display=0x%x Width=%d Height=%d BitsPerPixel=%d", Display, Width, Height, BitsPerPixel);
    /* Test if we have a valid display data structure pointer. */
    if (Display == NULL)
    {
        status = -1;
        gcmFOOTER();
        return status;
    }
    if ((Width <= 0) || (Height <= 0) || (BitsPerPixel <= 0))
    {
        status = -1;
        gcmFOOTER();
        return status;
    }

    do
    {
        BITMAPINFO info;
        void* bits;

        /* See if we need to get the default number of bits per pixel. */
        if (BitsPerPixel == 0)
        {
            BitsPerPixel = GetDeviceCaps(Display, BITSPIXEL);
        }

        /* Fill in the bitmap info structure. */
        info.bmiHeader.biSize          = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth         = Width;
        info.bmiHeader.biHeight        = -Height;
        info.bmiHeader.biPlanes        = 1;
        info.bmiHeader.biBitCount      = (WORD) BitsPerPixel;
        info.bmiHeader.biCompression   = BI_RGB;
        info.bmiHeader.biSizeImage     = 0;
        info.bmiHeader.biXPelsPerMeter = 0;
        info.bmiHeader.biYPelsPerMeter = 0;
        info.bmiHeader.biClrUsed       = 0;
        info.bmiHeader.biClrImportant  = 0;

        /* Create the bitmap. */
        *Pixmap = CreateDIBSection(Display,
            &info,
            DIB_RGB_COLORS,
            &bits,
            NULL,
            0);

        if (*Pixmap == NULL)
        {
            /* Break on bad bitmap. */
            _Error(TEXT("CreateDIBSection"));
            break;
        }

        /* Return pointer to the pixmap data structure. */
        gcmFOOTER_ARG("*Pixmap=0x%x", *Pixmap);
        return status;
    }
    while (0);

    /* Roll back on error. */
    if (*Pixmap != NULL)
    {
        DeleteObject(*Pixmap);
    }

    /* Error. */
    status = -16;
    gcmFOOTER();
    return status;
}

int
gcoOS_GetPixmapInfo(
    IN HALNativeDisplayType Display,
    IN HALNativePixmapType Pixmap,
    OUT int * Width,
    OUT int * Height,
    OUT int * BitsPerPixel,
    OUT int * Stride,
    OUT void* * Bits
    )
{
    DIBSECTION bitmap;
    int status = 0;
    gcmHEADER_ARG("Display=0x%x Pixmap=0x%x", Display, Pixmap);
    if (Pixmap == NULL)
    {
        /* Pixmap is not a valid pixmap data structure pointer. */
        status = -1;
        gcmFOOTER();
        return status;
    }

    /* Get the pixmap information. */
    if (GetObject(Pixmap, sizeof(bitmap), &bitmap) == 0)
    {
        _Error(TEXT("GetObject"));
        status = -1;
        gcmFOOTER();
        return status;
    }

    if (Width != NULL)
    {
        /* Return the pixmap width. */
        *Width = bitmap.dsBm.bmWidth;
    }

    if (Height != NULL)
    {
        /* Return the pixmap height. */
        *Height = bitmap.dsBm.bmHeight;
    }

    if (BitsPerPixel != NULL)
    {
        /* Return the pixmap color depth. */
        *BitsPerPixel = bitmap.dsBm.bmBitsPixel;
    }

    if (Stride != NULL)
    {
        /* Return the pixmap stride. */
        *Stride = bitmap.dsBm.bmWidthBytes;
    }

    if (Bits != NULL)
    {
        /* Return the pixmap bits. */
        *Bits = bitmap.dsBm.bmBits;
    }

    /* Success. */
    gcmFOOTER_NO();
    return status;
}

int
gcoOS_DestroyPixmap(
    IN HALNativeDisplayType Display,
    IN HALNativePixmapType Pixmap
    )
{
    /* Only process if we have a valid pointer */
    if (Pixmap != NULL)
    {
        /* Destroy the pixmap. */
        DeleteObject(Pixmap);
    }
    return 0;
}

int
gcoOS_DrawPixmap(
    IN HALNativeDisplayType Display,
    IN HALNativePixmapType Pixmap,
    IN int Left,
    IN int Top,
    IN int Right,
    IN int Bottom,
    IN int Width,
    IN int Height,
    IN int BitsPerPixel,
    IN void* Bits
    )
{
    DIBSECTION dib;

    /* Get bitmap information. */
    memset(&dib, 0, sizeof(dib));
    dib.dsBmih.biSize = sizeof(dib.dsBmih);

    if (GetObject(Pixmap, sizeof(dib), &dib) == 0)
    {
        /* Invalid bitmap. */
        return FALSE;
    }

    if (dib.dsBm.bmBits == NULL)
    {
        int ret = 0;
        BITFIELDINFO bfi;
        PBITMAPINFOHEADER bm = &bfi.bmi.bmiHeader;
        unsigned int *mask = (unsigned int*)(bm + 1);
        HGDIOBJ hBitmap = NULL;
        HDC hdcMem = NULL;

        do
        {
            hdcMem = CreateCompatibleDC(Display);
            if (hdcMem == NULL)
            {
                break;
            }

            hBitmap = SelectObject(hdcMem, Pixmap);
            if (hBitmap == NULL)
            {
                break;
            }

            memset(bm, 0, sizeof(BITMAPINFOHEADER));

            if (BitsPerPixel == 32)
            {
                mask[0] = 0x00FF0000;
                mask[1] = 0x0000FF00;
                mask[2] = 0x000000FF;
            }
            else if (BitsPerPixel == 16)
            {
                mask[0] = 0x0000F800;
                mask[1] = 0x000007E0;
                mask[2] = 0x0000001F;
            }
            else
            {
                break;
            }

            bm->biSize           = sizeof(bfi.bmi.bmiHeader);
            bm->biWidth          = Width;
            bm->biHeight         = -Height;
            bm->biPlanes         = 1;
            bm->biCompression    = BI_BITFIELDS;
            bm->biBitCount       = (WORD)BitsPerPixel;
            bm->biSizeImage      = (BitsPerPixel * Width * Height) << 3;
            bm->biXPelsPerMeter  = 0;
            bm->biYPelsPerMeter  = 0;
            bm->biClrUsed        = 0;
            bm->biClrImportant   = 0;

            ret = SetDIBitsToDevice(
                hdcMem,
                0, 0, Width, Height,
                0, 0, Top, Bottom - Top,
                Bits,
                (BITMAPINFO*)bm,
                DIB_RGB_COLORS
                ) ? TRUE : FALSE;

        } while (FALSE);

        if (hBitmap)
        {
            SelectObject(hdcMem, hBitmap);
        }

        if (hdcMem)
        {
            DeleteDC(hdcMem);
        }

        return (ret != 0) ? 0 : -1;
    }
    else
    {
        return -1;
    }
}

int
gcoOS_LoadEGLLibrary(
                     OUT void* * Handle
                     )
{
    WNDCLASS wndClass;
    int status = 0;
    /* Initialize the WNDCLASS structure. */
    ZeroMemory(&wndClass, sizeof(wndClass));
    wndClass.lpfnWndProc   = _WindowProc;
    wndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor       = LoadCursor(NULL, IDC_CROSS);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszClassName = TEXT("halClass");

    /* Register the window class. */
    regClass = RegisterClass(&wndClass);

    if (regClass == 0)
    {
        /* RegisterClass failed. */
        _Error(TEXT("RegisterClass"));
        status = -13;
        return status;
    }

    /* return gcoOS_LoadLibrary(NULL, "libEGL.dll", Handle); */
    *Handle = LoadLibrary("libOpenVG.dll");
    if (*Handle == NULL) return 1;

    return 0;
}

int
gcoOS_FreeEGLLibrary(
    IN void* Handle
    )
{
    if(regClass != 0)
    {
        /* Unregister the window class. */
        UnregisterClass(TEXT("halClass"), NULL);
    }
    /* return gcoOS_FreeLibrary(NULL, Handle); */
    FreeLibrary(Handle);
    return 0;
}

int
gcoOS_ShowWindow(
    IN HALNativeDisplayType Display,
    IN HALNativeWindowType Window
    )
{
    HWND window;

    if (Window == NULL)
    {
        return -1;
    }

    /* Get the window handle. */
    if (IsWindow((HWND) Window))
    {
        window = (HWND) Window;
    }
    else
    {
        return -2;
    }

    /* Show the window. */
    ShowWindow(window, SW_SHOWNORMAL);

    /* Initial paint of the window. */
    UpdateWindow(window);

    return 0;
}

int
gcoOS_HideWindow(
    IN HALNativeDisplayType Display,
    IN HALNativeWindowType Window
    )
{
    HWND window;

    if (Window == NULL)
    {
        return -1;
    }

    /* Get the window handle. */
    if (IsWindow((HWND) Window))
    {
        window = (HWND) Window;
    }
    else
    {
        return -2;
    }

    /* Hide the window. */
    ShowWindow(window, SW_HIDE);
    return 0;
}

int
gcoOS_SetWindowTitle(
    IN HALNativeDisplayType Display,
    IN HALNativeWindowType Window,
    IN const char* Title
    )
{
    HWND window;
#ifdef UNICODE
    /* Temporary buffer. */
    LPTSTR title;

    /* Number of characters required for the temporary buffer. */
    int count;
#endif

    if (IsWindow((HWND) Window))
    {
        window = (HWND) Window;
    }
    else
    {
        return -2;
    }

#ifdef UNICODE
    /* Query number of characters required for the temporary buffer. */
    count = MultiByteToWideChar(CP_ACP,
                                MB_PRECOMPOSED,
                                Title, -1,
                                NULL, 0);

    /* Allocate the temporary buffer. */
    title = (LPTSTR) malloc(count * sizeof(TCHAR));

    /* Only process if the allocation succeeded. */
    if (title != NULL)
    {
        /* Convert the title into UNICODE. */
        MultiByteToWideChar(CP_ACP,
                            MB_PRECOMPOSED,
                            Title, -1,
                            title, count);

        /* Set the window title. */
        SetWindowText(window, title);

        /* Free the temporary buffer. */
        free(title);
    }
#else
    /* Set the window title. */
    SetWindowText(window, Title);
#endif
    return 0;
}

int
gcoOS_CapturePointer(
    IN HALNativeDisplayType Display,
    IN HALNativeWindowType Window
    )
{
    if (Window == NULL)
    {
        ReleaseCapture();
        ClipCursor(NULL);
    }
    else
    {
        RECT rect;
        POINT ul, br;
        HWND window;

        if (IsWindow((HWND) Window))
        {
            window = (HWND) Window;
        }
        else
        {
            return -2;
        }

        GetClientRect(window, &rect);

        ul.x = rect.left;
        ul.y = rect.right;
        ClientToScreen(window, &ul);

        br.x = rect.right  + 1;
        br.y = rect.bottom + 1;
        ClientToScreen(window, &br);

        SetRect(&rect, ul.x, ul.y, br.x, br.y);

        SetCapture(window);
        ClipCursor(&rect);
    }
    return 0;
}

int
gcoOS_GetEvent(
    IN HALNativeDisplayType Display,
    IN HALNativeWindowType Window,
    OUT halEvent * Event
    )
{
    /* Message. */
    MSG msg;

    /* Translated scancode. */
    halKeys scancode;

    /* Test for valid Window and Event pointers. */
    if ((Window == NULL) || (Event == NULL))
    {
        return -1;
    }

    /* Loop while there are messages in the queue for the window. */
    while (PeekMessage(&msg, (HALNativeWindowType)Window, 0, 0, PM_REMOVE))
    {
        switch (msg.message)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
            /* Keyboard event. */
            Event->type = HAL_KEYBOARD;

            /* Translate the scancode. */
            scancode = (msg.wParam & 0x80)
                     ? keys[msg.wParam & 0x7F].extended
                     : keys[msg.wParam & 0x7F].normal;

            /* Set scancode. */
            Event->data.keyboard.scancode = scancode;

            /* Set ASCII key. */
            Event->data.keyboard.key = (  (scancode < HAL_SPACE)
                                       || (scancode >= HAL_F1)
                                       )
                                       ? 0
                                       : (char) scancode;

            /* Set up or down flag. */
            Event->data.keyboard.pressed = (msg.message == WM_KEYDOWN);

            /* Valid event. */
            return 0;

        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT:
            /* Application should close. */
            Event->type = HAL_CLOSE;

            /* Valid event. */
            return 0;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            /* Button event. */
            Event->type = HAL_BUTTON;

            /* Set button states. */
            Event->data.button.left   = (msg.wParam & MK_LBUTTON) ? 1 : 0;
            Event->data.button.middle = (msg.wParam & MK_MBUTTON) ? 1 : 0;
            Event->data.button.right  = (msg.wParam & MK_RBUTTON) ? 1 : 0;
            Event->data.button.x      = LOWORD(msg.lParam);
            Event->data.button.y      = HIWORD(msg.lParam);

            /* Valid event. */
            return 0;

        case WM_MOUSEMOVE:
            /* Pointer event.*/
            Event->type = HAL_POINTER;

            /* Set pointer location. */
            Event->data.pointer.x = LOWORD(msg.lParam);
            Event->data.pointer.y = HIWORD(msg.lParam);

            /* Valid event. */
            return 0;

        default:
            /* Translate and dispatch message. */
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            break;
        }
    }

    /* Test if the window is still valid. */
    if (!IsWindow((HALNativeWindowType)Window))
    {
        /* Application should close. */
        Event->type = HAL_CLOSE;

        /* Valid event. */
        return 0;
    }

    /* No event pending. */
    return -19;
}

#define gcdSUPPORT_EXTERNAL_IMAGE_EXT 0

/* GL_VIV_direct_texture */
#ifndef GL_VIV_direct_texture
#define GL_VIV_YV12                        0x8FC0
#define GL_VIV_NV12                        0x8FC1
#define GL_VIV_YUY2                        0x8FC2
#define GL_VIV_UYVY                        0x8FC3
#define GL_VIV_NV21                        0x8FC4
#endif

int
gcoOS_CreateClientBuffer(
    IN int Width,
    IN int Height,
    IN int Format,
    IN int Type,
    OUT void* * ClientBuffer
    )
{
    int status = -13;
#if gcdSUPPORT_EXTERNAL_IMAGE_EXT
    do
    {
        gcoSURF surf;
        int format;

        switch (Format)
        {
        case GL_VIV_YUY2:
            format = gcvSURF_YUY2;
            break;
        case GL_VIV_UYVY:
            format = gcvSURF_UYVY;
            break;
        case GL_VIV_NV12:
            format = gcvSURF_NV12;
            break;
        case GL_VIV_NV21:
            format = gcvSURF_NV21;
            break;
        case GL_VIV_YV12:
            format = gcvSURF_YV12;
            break;
        default:
            return -1;
        }
        status = gcoSURF_Construct(NULL,
                          Width,
                          Height,
                          1,
                          gcvSURF_BITMAP,
                          format,
                          gcvPOOL_SYSTEM,
                          &surf);
        if(status != 0)
        {
            break;
        }
        *ClientBuffer = (void*)surf;
        /* Return pointer to the pixmap data structure. */
        status = 0;
    }
    while (0);
#endif

    /* Error. */
    return status;
}

int
gcoOS_GetClientBufferInfo(
    IN void* ClientBuffer,
    OUT int * Width,
    OUT int * Height,
    OUT int * Stride,
    OUT void* * Bits
    )
{
    int status = -13;
#if gcdSUPPORT_EXTERNAL_IMAGE_EXT
    status = -1;
    if (gcoSURF_IsValid((gcoSURF)ClientBuffer) != gcvSTATUS_TRUE)
    {
        return status;
    }

    if (Width || Height)
    {
        status = gcoSURF_GetSize((gcoSURF)ClientBuffer,
                                 (unsigned int*)Width,
                                 (unsigned int*)Height,
                                 NULL);

        if (status != 0)
        {
            return status;
        }
    }

    if (Stride)
    {
        status = gcoSURF_GetAlignedSize((gcoSURF)ClientBuffer,
                                         NULL,
                                         NULL,
                                         Stride);

        if (status != 0)
        {
            return status;
        }
    }

    if (Bits)
    {
        void* memory[3];

        status = gcoSURF_Lock((gcoSURF)ClientBuffer,
                              NULL,
                              memory);

        if (status != 0)
        {
            return status;
        }

        *Bits = memory[0];

    }

    return status;
#else
    return status;
#endif
}

int
gcoOS_DestroyClientBuffer(
    IN void* ClientBuffer
    )
{
#if gcdSUPPORT_EXTERNAL_IMAGE_EXT
    return gcoSURF_Destroy((gcoSURF)ClientBuffer);
#else
    return -13;
#endif
}

int
gcoOS_GetWindowInfoEx(
    IN HALNativeDisplayType Display,
    IN HALNativeWindowType Window,
    OUT int * X,
    OUT int * Y,
    OUT int * Width,
    OUT int * Height,
    OUT int * BitsPerPixel,
    OUT unsigned int * Offset,
    OUT int * Format,
    OUT int * Type
    )
{
    RECT rect;
    INT bitsPerPixel;
    int format;

    /* Get device context bit depth. */
    bitsPerPixel = GetDeviceCaps(Display, BITSPIXEL);

    /* Return format for window depth. */
    switch (bitsPerPixel)
    {
    case 16:
        /* 16-bits per pixel. */
        format = 209;
        break;

    case 32:
        /* 32-bits per pixel. */
        format = 212;
        break;

    default:
        /* Unsupported colot depth. */
        return -1;
    }

    ShowWindow( Window, SW_SHOWNORMAL );

    /* Query window client rectangle. */
    if (!GetClientRect(Window, &rect))
    {
        /* Error. */
        return -1;
    }

    /* Set the output window parameters. */
    if (X != NULL)
    {
        *X = rect.left;
    }

    if (Y != NULL)
    {
        *Y = rect.top;
    }

    if (Width != NULL)
    {
        *Width = rect.right  - rect.left;
    }

    if (Height != NULL)
    {
        *Height = rect.bottom - rect.top;
    }

    if (BitsPerPixel != NULL)
    {
        *BitsPerPixel = bitsPerPixel;
    }

    if (Format != NULL)
    {
        *Format = format;
    }

    if (Type != NULL)
    {
        *Type = 6;
    }

    /* Success. */
    return 0;
}

/*******************************************************************************
**
**  gcoOS_GetProcAddress
**
**  Get the address of a function inside a loaded library.
**
**  INPUT:
**
**      gcoOS Os
**          Pointer to gcoOS object.
**
**      gctHANDLE Handle
**          Handle of a loaded libarry.
**
**      gctCONST_STRING Name
**          Name of function to get the address of.
**
**  OUTPUT:
**
**      gctPOINTER * Function
**          Pointer to variable receiving the function pointer.
*/
int
gcoOS_GetProcAddress(
    IN void* Os,
    IN void* Handle,
    IN const char* Name,
    OUT void** Function
)
{
    int status;

    /* Get the address of the function. */
    *Function = (void*)(GetProcAddress(Handle, Name));

    /* Return error if function could not be found. */
    status = (*Function == NULL) ? 1 : 0;

    return status;
}

/*******************************************************************************
**
**  gcoOS_GetTicks
**
**  Get the number of milliseconds since the system started.
**
**  INPUT:
**
**  OUTPUT:
**
*/
int
gcoOS_GetTicks(
    void
)
{
    /* Return the OS tick count. */
    return GetTickCount();
}
