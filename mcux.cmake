if(CONFIG_MCUX_COMPONENT_middleware.openvg)
    mcux_add_source(
        SOURCES src/vg_api.c
                src/vg_context.c
                src/vg_egl.c
                src/vg_image.c
                src/vg_vgu.c
                src/vg_egl_freertos.c
                include/EGL/egl.h
                include/EGL/eglplatform.h
                include/EGL/eglvivante.h
                include/KHR/khrplatform.h
                include/VG/openvg.h
                include/VG/vgplatform.h
                include/VG/vgu.h
                include/Vivante/vg_context.h
                include/Vivante/vg_defs.h
                include/Vivante/vg_image.h
                include/Vivante/vg_math.h
        BASE_PATH ${SdkRootDirPath}/middleware/openvg
    )

    mcux_add_include(
        INCLUDES include
                 include/Vivante
                 include/EGL
                 include/VG
        BASE_PATH ${SdkRootDirPath}/middleware/openvg
    )

    mcux_add_configuration(
        TOOLCHAINS armgcc
        CC "-Wno-enum-compare"
    )
endif()
