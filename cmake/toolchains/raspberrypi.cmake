# Targets: Raspberry Pi
# Requires: x86 Linux host.
#
# OcherBook defaults to expecting toolchains in a common location, with a
# somewhat standardized naming scheme (although you can override this with the
# OCHER_TOOLCHAIN_ROOT environment variable).
#
# The Raspberry Pi foundation has some prebuilt toolchains here:
# 	https://github.com/raspberrypi/tools/tree/master/arm-bcm2708
#
# Summary instructions adapted for OcherBook:
# 	$ git clone ...
# 	$ TODO
#
# Finally, configure and build with the toolchain:
# 	$ cd OcherBook
# 	$ mkdir build
# 	$ cd build
# 	$ cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/raspberrypi.cmake ..
# 	$ make


set(OCHER_TARGET raspberrypi)
set(OCHER_TARGET_RASPBERRYPI TRUE)

set(WITH_FREETYPE_DEFAULT On)
set(WITH_FRAMEBUFFER_DEFAULT On)

set(CMAKE_SYSTEM_NAME Linux)

if("$ENV{OCHER_TOOLCHAIN_ROOT}" STREQUAL "")
	set(OCHER_TOOLCHAIN_ROOT "/opt/toolchains/arm-bcm2708hardfp-linux-gnuabi/")
else()
	set(OCHER_TOOLCHAIN_ROOT "$ENV{OCHER_TOOLCHAIN_ROOT}")
endif()
message("Using raspberrypi toolchain in ${OCHER_TOOLCHAIN_ROOT}")
set(CMAKE_C_COMPILER      "${OCHER_TOOLCHAIN_ROOT}/bin/arm-bcm2708hardfp-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER    "${OCHER_TOOLCHAIN_ROOT}/bin/arm-bcm2708hardfp-linux-gnueabi-g++")
set(CMAKE_FIND_ROOT_PATH  "${OCHER_TOOLCHAIN_ROOT}/arm-bcm2708hardfp-linux-gnueabi/")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
