cmake_minimum_required(VERSION 3.13)

project(OpenVG)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SIZEOF_VOID_P 4)
include(GNUInstallDirs)

set(PROJ_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(vendor/nxp)
