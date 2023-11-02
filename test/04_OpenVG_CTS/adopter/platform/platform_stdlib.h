#ifndef PLATFORM_STDLIB_H
#define PLATFORM_STDLIB_H

#ifdef __cplusplus 
extern "C" { 
#endif

/*
** To be adapted to target platform.
*/

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#    ifndef UNDER_CE
#        include <direct.h>
#    endif
#endif

#ifndef ANSWER_DEFAULT_DIR
#define ANSWER_DEFAULT_DIR               "./data/answer"
#endif
#ifndef INFO_DEFAULT_DIR
#define INFO_DEFAULT_DIR                 "./data/info"
#endif
#define TEST_OPTION_VGU                  1

/* stdarg */
#define VA_ARG(arg, type)                va_arg((arg), type)
#define VA_END(arg)                      va_end((arg))
#define VA_LIST                          va_list
#define VA_START(arg, format)            va_start((arg), (format))

/* stdio */
typedef FILE                            *FILEHANDLE;
#define FCLOSE                           fclose
#define FEOF                             feof
#define FFLUSH                           fflush
#define FOPEN                            fopen
#define FPUTS                            fputs
#define PUTS                             puts
#define PRINTF                           printf
#define FPRINTF                          fprintf
#define SPRINTF                          sprintf
#define VPRINTF                          vprintf
#define VSPRINTF                         vsprintf
#define FSCANF                           fscanf
#define SSCANF                           sscanf
#define FWRITE                           fwrite
#ifdef WIN32
#define MKDIR                            _mkdir
#else
#define MKDIR(x)                         mkdir(x, 0777)
#endif
#define RMDIR                            rmdir
/* stdlib */
#define EXIT                             exit
#define FREE                             free
#define MALLOC                           malloc
#define MEMCPY                           memcpy
#define MEMSET                           memset

/* string */
#define STRCMP                           strcmp
#define STRCPY                           strcpy
#define STRLEN                           strlen
#define STRSTR                           strstr
#define STRCAT                           strcat
#define ITOA                             itoa
#ifdef __cplusplus 
} /* extern "C" */
#endif

#endif /* PLATFORM_STDLIB_H */
