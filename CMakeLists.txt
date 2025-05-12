project(OpenVG)
include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)
cmake_minimum_required(VERSION 3.13)

enable_language(ASM)

set(PROJ_DIR    ${CMAKE_CURRENT_SOURCE_DIR})

option(OPENVG_DBG_INFO   "Include debugging information in the binary." OFF)

if(CMAKE_BUILD_TYPE STREQUAL SDRAM_DEBUG)
    add_definitions(-DOPENVG_DEBUG_BUILD=1)
    include (sdram_debug.cmake)
endif()
if(CMAKE_BUILD_TYPE STREQUAL SDRAM_RELEASE)
    add_definitions(-DOPENVG_RELEASE_BUILD=1)
    include (sdram_release.cmake)
endif()

###############################################################################
# libOpenVG
###############################################################################
add_library(
    OpenVG
    STATIC
    ${PROJ_DIR}/src/vg_api.c
    ${PROJ_DIR}/src/vg_context.c
    ${PROJ_DIR}/src/vg_egl.c
    ${PROJ_DIR}/src/vg_image.c
    ${PROJ_DIR}/src/vg_vgu.c
    ${PROJ_DIR}/src/vg_egl_freertos.c
)

target_include_directories(
    OpenVG
    PRIVATE
    ${PROJ_DIR}/bsp/${MCU_SDK_VER}
    ${PROJ_DIR}/include
    ${PROJ_DIR}/include/Vivante
    ${PROJ_DIR}/include/EGL
    ${PROJ_DIR}/include/VG

    ${VG_LITE_DIR}/include

    ${MCU_SDK_DIR}/rtos/freertos/freertos-kernel/include
    ${MCU_SDK_DIR}/rtos/freertos/freertos-kernel/portable/GCC/ARM_CM4F
    ${MCU_SDK_DIR}/CMSIS/Core/Include/
    ${MCU_SDK_DIR}/devices/MIMXRT1176
    ${MCU_SDK_DIR}/devices/MIMXRT1176/drivers
    ${MCU_SDK_DIR}/components/video/
    ${MCU_SDK_DIR}/components/video/display/fbdev/
    ${MCU_SDK_DIR}/components/video/display/dc
)

