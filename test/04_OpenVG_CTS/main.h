#ifndef __MAIN_H__
#define __MAIN_H__

#include "app.h"
#include "adopter/native/native.h"
#include "adopter/platform/platform.h"

#if !defined(ANSWER_DEFAULT_DIR) 
    #error ANSWER_DEFAULT_DIR is not defined (-DANSWER_DEFAULT_DIR="PathName")
#endif
#if !defined(INFO_DEFAULT_DIR) 
    #error INFO_DEFAULT_DIR is not defined (-DINFO_DEFAULT_DIR="PathName")
#endif
#if !defined(TEST_OPTION_VGU) 
    #error TEST_OPTION_VGU is not defined (-DTEST_OPTION_VGU=1 or 0)
#endif

#define CHIP_MODEL 355

#define CT_UNSPECIFIED          -99
#define CT_MAX_FILE_NAME        200

#define CT_RELEASE_TITLE        "OpenVG Conformance Test"
#define CT_RELEASE_NAME         "OnDevice"
#define CT_RELEASE_VERSION      "gc555"
#define CT_RELEASE_DATE         "2023-01-03"
#define CT_NULL                 101001

#define CT_MAX_SCISSOR_RECT     32
#define CT_MAX_DASH_COUNT       16
#define CT_MAX_COLOR_RAMP_STOPS 32

typedef struct _CT_FuncRec {
    char      name[30];
    long      number;
    CT_Result (*func)(void);          
} CT_FuncRec;

typedef enum {
    FILE_INVALID_TYPE = 0,
    FILE_DAT_TYPE     = 1,
    FILE_TGA_TYPE     = 2
} CT_FileType;

typedef struct _CT_File {
    CT_FileType type;
    VGbyte      filename[CT_MAX_FILE_NAME];
    CT_AppRec  *pMe;
} CT_File;

#ifdef __cplusplus 
extern "C" { 
#endif

/* main.c */
//extern void Output(VGint level, const VGbyte *format, ...);
//extern TK_ConfigInfoRec *GetConfigInfo( void );

/* driver.c */
extern CT_Result Driver(CT_AppRec *pMe);
extern CT_Result DriverSetup(CT_AppRec *pMe);
extern CT_Result DriverSetupName(CT_AppRec *pMe, VGbyte *code);
extern CT_Result DriverSetupFile(CT_AppRec *pMe, VGbyte *fileName);
extern CT_Result DriverSaveTestList(CT_AppRec *pMe, FILEHANDLE file);

#ifdef __cplusplus 
} /* extern "C" */
#endif

#endif /* __MAIN_H__ */
