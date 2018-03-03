# Targets: Kobo (Touch, ...?)
# Requires: x86 Linux host.
#
# Obtain the toolchain:
#   $ git clone https://github.com/kobolabs/Kobo-Reader.git
#
# Install it to some common location with a subdirectory named after this file:
#   $ mkdir -p ~/toolchains
#   $ tar -C ~/toolchains -Jxf Kobo-Reader/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2013.04-20130417_linux.tar.bz2
#   $ cd ~/toolchains
#   $ ln -s Kobo-Reader/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2013.04-20130417_linux/ kobo
#
# Finally, configure and build with the toolchain:
#   $ cd OcherBook
#   $ mkdir build
#   $ cd build
#   $ cmake -DOCHER_TOOLCHAIN_ROOT=$HOME/toolchains -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/kobo.cmake ..
#   $ make


set(OCHER_TARGET kobo)
set(OCHER_TARGET_KOBO TRUE)

set(WITH_FREETYPE_DEFAULT ON)
set(WITH_FRAMEBUFFER_DEFAULT ON)
set(WITH_SYSTEM_LIBEV_DEFAULT OFF)

set(CMAKE_SYSTEM_NAME Linux)

message("Using toolchain in ${OCHER_TOOLCHAIN_ROOT}/${OCHER_TARGET}")
set(CMAKE_C_COMPILER      "${OCHER_TOOLCHAIN_ROOT}/${OCHER_TARGET}/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER    "${OCHER_TOOLCHAIN_ROOT}/${OCHER_TARGET}/bin/arm-linux-gnueabihf-g++")
set(CMAKE_FIND_ROOT_PATH  "${OCHER_TOOLCHAIN_ROOT}/${OCHER_TARGET}/arm-linux-gnueabihf/")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
