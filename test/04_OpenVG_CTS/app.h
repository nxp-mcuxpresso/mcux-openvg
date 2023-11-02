#ifndef __APP_H__
#define __APP_H__

#include <EGL/egl.h>
#include <VG/openvg.h>
#include <VG/vgu.h>

#define TK_MAX_CONFIG       100
#define CT_MAX_DRIVER       500

#define WINDSIZEX           64
#define WINDSIZEY           64

typedef enum {
    CT_NO_ERROR = 0,
    CT_ERROR = 1
} CT_Result;

/* Toolkit Configs Structure */
typedef struct _TK_ConfigsRec {
    long count;
    long conformantCount;
    long IDs[TK_MAX_CONFIG];
    long type[TK_MAX_CONFIG];
} TK_ConfigsRec;

typedef struct _TK_ConfigInfoRec {
    EGLint   configID;
    EGLint   rDepth;
    EGLint   gDepth;
    EGLint   bDepth;
    EGLint   aDepth;
    EGLint   lDepth;
    EGLint   maskSize;
    EGLint   cSpace;
    EGLint   alphaMult;
    EGLint   sampleBuffers;
    EGLint   samples;
} TK_ConfigInfoRec;

typedef struct _CT_DriverRec {
    VGbyte      code[10];
    CT_Result(*func)();
    VGuint      nsubtests;
} CT_DriverRec;

typedef struct _CT_AppRec {
    char              title[80];
    char              name[80];
    char              version[40];
    char              date[40];
    VGint             verboseLevel;
    VGint             configID;

    TK_ConfigsRec     list;
    TK_ConfigInfoRec  info;

    NativeWindowType  native_window;
    NativeDisplayType native_display;

    EGLDisplay        egl_display;
    EGLSurface        egl_surface;
    EGLContext        egl_context;

    VGint             testList[CT_MAX_DRIVER];
    CT_DriverRec*     curDriver;
} CT_AppRec;

#ifdef __cplusplus 
extern "C" {
#endif

#ifdef __cplusplus 
} /* extern "C" */
#endif

#endif /* __APP_H__ */
