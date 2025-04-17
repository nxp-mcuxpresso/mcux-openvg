#!/bin/sh

set -e

CWD="`dirname $(realpath $0)`"
SCRIPTDIR="$(realpath $CWD)"

export G_MODE='Unix Makefiles'

export ARMGCC_DIR=/home/nxa07475/work/toolchains/gcc-arm-none-eabi-10.3-2021.10/
export VG_LITE_DIR=/home/nxa07475/work/gpu-vglite/__install_debug_rt700/
export MCU_SDK_DIR=/home/nxa07475/mcuxpresso/02/SDKPackages/MIMXRT700-EVK
export PLATFORM=rt700

export BUILD_TYPE=debug
export BUILD_DIR=__build_${BUILD_TYPE}_${PLATFORM}
export INSTALL_DIR=__install_${BUILD_TYPE}_${PLATFORM}

cd $SCRIPTDIR

rm -rf "${BUILD_DIR}"
rm -rf "${INSTALL_DIR}"

cmake \
  -G "$G_MODE"                      \
  -DCMAKE_TOOLCHAIN_FILE="$MCU_SDK_DIR/tools/cmake_toolchain_files/armgcc.cmake" \
  -DCMAKE_BUILD_TYPE=${BUILD_TYPE}    \
  -DOPENVG_PLATFORM=${PLATFORM}       \
  -DMCU_SDK_DIR=${MCU_SDK_DIR}        \
  -DVG_LITE_DIR=${VG_LITE_DIR}        \
  -B ${BUILD_DIR}

cmake --build ${BUILD_DIR}
cmake --install ${BUILD_DIR} --prefix ${INSTALL_DIR}
