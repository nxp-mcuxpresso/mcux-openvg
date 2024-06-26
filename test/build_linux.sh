#!/bin/bash
                                                                                               
usage()
{
    echo
    echo "Usage: $0 BOARD"
    echo
    echo "    BOARD: X86 PCIE-GEN6 IMX6Q35"
    echo
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

BOARD=$1
case "$BOARD" in

PCIE-GEN6)
    export OPENVG_LOC=/data/openvg/cl/vgmark-555/OpenVG/build
    export TOOLCHAIN=/usr
    export CROSS_COMPILE=""
;;

X86)
    export OPENVG_LOC=/data/openvg/cl/vgmark-555/OpenVG/build
    export TOOLCHAIN=/usr
    export CROSS_COMPILE=""
;;

IMX6Q35)
    export OPENVG_LOC=/data/openvg/cl/vgmark-555/OpenVG/build
    source /home/software/Linux/freescale/L5.15.52_RC2_20220919/Toolchain/32/environment-setup-cortexa9t2hf-neon-poky-linux-gnueabi
;;

*)
    echo
    echo "ERROR: Unknown [ $BOARD ], or not support so far."
    usage
    exit
;;
esac;

cd ./tiger
make -f makefile.null clean
make -f makefile.null

cd ../01_ClearTarget
make -f makefile.null clean
make -f makefile.null

cd ../02_LetterA
make -f makefile.null clean
make -f makefile.null

cd ../03_TSOverflow
make -f makefile.null clean
make -f makefile.null

