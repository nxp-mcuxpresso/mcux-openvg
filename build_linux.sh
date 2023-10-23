#!/bin/bash
                                                                                               
usage()
{
    echo
    echo "Usage: $0 BOARD"
    echo
    echo "    BOARD:"
    echo "       X86 PCIE-GEN6 IMX6_Q35"
    echo
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

export AQROOT=`pwd`
export SDK_DIR=$AQROOT/build

BOARD=$1
case "$BOARD" in

PCIE-GEN6)
    export TOOLCHAIN=/usr
    export CROSS_COMPILE=""
    export EGL_API_FBDEV=1
    export CPU_ARCH=0
    export ARCH=x86
    export VG_LITE_SDK=$AQROOT/../Hubi.dev/build
;;

X86)
    export TOOLCHAIN=/usr
    export CROSS_COMPILE=""
    export EGL_API_FBDEV=1
    export CPU_ARCH=0
    export ARCH=x86
    export VG_LITE_SDK=$AQROOT/../Hubi.dev/build
;;

IMX6_Q35)
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
    exit
;;
esac;

cd $AQROOT
make -f makefile.linux clean
make -f makefile.linux install

cp $VG_LITE_SDK/drivers/*  $SDK_DIR/drivers
cp $VG_LITE_SDK/inc/*      $SDK_DIR/inc


