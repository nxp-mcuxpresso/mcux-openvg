#!/bin/bash
                                                                                               
usage()
{
    echo
    echo "Usage: ./$0 install/clean BOARD"
    echo
    echo "    BOARD:"
    echo "       X86 PCIE-GEN6 ZC702"
    echo
}

if [ $# -lt 2 ]; then
    usage
    exit 1
fi

export AQROOT=`pwd`
export SDK_DIR=$AQROOT/build

BOARD=$2
case "$BOARD" in

ZC702)
    export CROSS_COMPILE=/home/software/Linux/zync/arm-vivante-linux-gnueabihf/bin/arm-vivante-linux-gnueabihf-
    export SYSROOT_DIR=/home/software/Linux/zync/arm-vivante-linux-gnueabihf/arm-vivante-linux-gnueabihf/sysroot
    export KERNEL_DIR=/home/software/Linux/zync/git/linux-s2c
    export CPU_ARCH=armv7-a
    export ARCH=arm
    export ENABLE_PCIE=0
    export EGL_API_FBDEV=1
    export VG_LITE_SDK=$AQROOT/../Hubi.dev/build
;;

PCIE-GEN6)
    export TOOLCHAIN=/usr
    export CROSS_COMPILE=""
    export KERNEL_DIR=/home/software/Linux/x86_pcie/linux-headers-4.8.0-41-generic/
    export ENABLE_PCIE=1
    export EGL_API_FBDEV=1
    export CPU_ARCH=0
    export ARCH=x86_64
    export VG_LITE_SDK=$AQROOT/../Hubi.dev/build
;;

X86)
    export TOOLCHAIN=/usr
    export CROSS_COMPILE=""
    export KERNEL_DIR=/home/software/Linux/x86_pcie/linux-headers-4.8.0-41-generic/
    export ENABLE_PCIE=1
    export EGL_API_FBDEV=1
    export CPU_ARCH=0
    export ARCH=x86
    export VG_LITE_SDK=$AQROOT/../Hubi.dev/build
;;

IMX6_Q35)
    export KERNEL_DIR=/home/software/Linux/freescale/L5.15.52_RC2_20220919/Kernel/32/linux-lts-nxp
    export TOOLCHAIN=/home/software/Linux/freescale/L5.15.52_RC2_20220919/Toolchain/32/sysroots/x86_64-pokysdk-linux
    export CROSS_COMPILE=/home/software/Linux/freescale/L5.15.52_RC2_20220919/Toolchain/32/sysroots/x86_64-pokysdk-linux/usr/bin/arm-poky-linux-gnueabi/arm-poky-linux-gnueabi-
    export SYSROOT_DIR=/home/software/Linux/freescale/L5.15.52_RC2_20220919/Toolchain/32/sysroots/cortexa9t2hf-neon-poky-linux-gnueabi
    export ROOT_USR=$SYSROOT_DIR/usr
    export CPU_TYPE=cortex-a9
    export CPU_ARCH=0
    export ARCH=arm
    export VG_LITE_SDK=$AQROOT/../Hubi.dev/build
    export ENABLE_PCIE=0
    export EGL_API_WL=1
    export CFLAGS="--sysroot=$SYSROOT_DIR"
    export LDFLAGS="--sysroot=$SYSROOT_DIR"
    export PFLAGS="--sysroot=$SYSROOT_DIR" 

    source /home/software/Linux/freescale/L5.15.52_RC2_20220919/Toolchain/32/environment-setup-cortexa9t2hf-neon-poky-linux-gnueabi
    export YOCTO_BUILD=1
    
;;

*)
    echo
    echo "ERROR: Unknown [ $BOARD ], or not support so far."
    usage
;;
esac;

BUILD_OPTIONS="EGL_API_WL=$EGL_API_WL"
BUILD_OPTIONS="$BUILD_OPTIONS EGL_API_FBDEV=$EGL_API_FBDEV"

cd $AQROOT; make -f makefile.linux $BUILD_OPTIONS $1 2>&1 |tee $AQROOT/linux_build.log
