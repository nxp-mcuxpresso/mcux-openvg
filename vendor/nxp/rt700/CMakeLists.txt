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
    ${PROJ_DIR}/include
    ${PROJ_DIR}/include/Vivante
    ${PROJ_DIR}/include/EGL
    ${PROJ_DIR}/include/VG

    ${PROJ_DIR}/vendor/nxp/${OPENVG_PLATFORM}

    ${VG_LITE_DIR}/include
    ${VG_LITE_DIR}/include/inc

    ${MCU_SDK_DIR}/rtos/freertos/freertos-kernel/include
    ${MCU_SDK_DIR}/rtos/freertos/freertos-kernel/portable/GCC/ARM_CM33_NTZ/non_secure
    ${MCU_SDK_DIR}/CMSIS/Core/Include/
    ${MCU_SDK_DIR}/devices/MIMXRT798S
    ${MCU_SDK_DIR}/devices/MIMXRT798S/periph
    ${MCU_SDK_DIR}/devices/MIMXRT798S/drivers
    ${MCU_SDK_DIR}/components/video/
    ${MCU_SDK_DIR}/components/video/display/fbdev/
    ${MCU_SDK_DIR}/components/video/display/dc
    ${MCU_SDK_DIR}/boards/mimxrt700evk/openvg_examples/openvg_tiger/cm33_core0
)

include (flags.cmake)

install(
    DIRECTORY ${PROJ_DIR}/include/Vivante
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${PROJ_DIR}/include/EGL
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${PROJ_DIR}/include/VG
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${PROJ_DIR}/include/KHR
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

install(
    TARGETS OpenVG
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
