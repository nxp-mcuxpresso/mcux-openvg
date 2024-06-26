#!/bin/sh

set -e

CWD="`dirname $(realpath $0)`"
SCRIPTDIR="$(realpath $CWD)"

export G_MODE='Unix Makefiles'

export ARMGCC_DIR=/home/nxa07475/work/toolchains/gcc-arm-none-eabi-10.3-2021.10/
export VG_LITE_DIR=/home/nxa07475/work/gpu-vglite/__install_release_rt1170/
export MCU_SDK_DIR=/home/nxa07475/mcuxpresso/02/SDKPackages/SDK_2_15_000_MIMXRT1170-EVK/
export MCU_SDK_VER=2.15.x

export BUILD_TYPE=SDRAM_RELEASE

cd $SCRIPTDIR

rm -rf __build

cmake \
  -G "$G_MODE"                      \
  -DCMAKE_TOOLCHAIN_FILE="$MCU_SDK_DIR/tools/cmake_toolchain_files/armgcc.cmake" \
  -DCMAKE_BUILD_TYPE=$BUILD_TYPE    \
  -DMCU_SDK_DIR=$MCU_SDK_DIR        \
  -DMCU_SDK_VER=$MCU_SDK_VER        \
  -DVG_LITE_DIR=$VG_LITE_DIR        \
  -B __build

make -C __build

