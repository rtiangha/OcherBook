# Targets: Raspberry Pi
# Requires: x86 Linux host.
#
# The Raspberry Pi foundation has some prebuilt toolchains here:
# 	https://github.com/raspberrypi/tools/tree/master/arm-bcm2708
# 	$ git clone ...
# 	$ TODO
#
# Install it to some common location with a subdirectory named after this file:
#   $ mkdir -p ~/toolchains
# 	$ TODO
#
# Finally, configure and build with the toolchain:
# 	$ cd OcherBook
# 	$ mkdir build
# 	$ cd build
#   $ cmake -DOCHER_TOOLCHAIN_ROOT=$HOME/toolchains -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/raspberrypi.cmake ..
# 	$ make


set(OCHER_TARGET raspberrypi)
set(OCHER_TARGET_RASPBERRYPI TRUE)

set(WITH_FREETYPE_DEFAULT ON)
set(WITH_FRAMEBUFFER_DEFAULT ON)
set(WITH_SYSTEM_LIBEV_DEFAULT OFF)

set(CMAKE_SYSTEM_NAME Linux)

message("Using toolchain in ${OCHER_TOOLCHAIN_ROOT}/${OCHER_TARGET}")
set(CMAKE_C_COMPILER      "${OCHER_TOOLCHAIN_ROOT}/${OCHER_TARGET}/bin/arm-bcm2708hardfp-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER    "${OCHER_TOOLCHAIN_ROOT}/${OCHER_TARGET}/bin/arm-bcm2708hardfp-linux-gnueabi-g++")
set(CMAKE_FIND_ROOT_PATH  "${OCHER_TOOLCHAIN_ROOT}/${OCHER_TARGET}/arm-bcm2708hardfp-linux-gnueabi/")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
