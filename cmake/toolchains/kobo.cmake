# Targets: Kobo (Touch, ...?)
# Requires: x86 Linux host.
#
# OcherBook defaults to expecting toolchains in a common location, with a
# somewhat standardized naming scheme (although you can override this with the
# OCHER_TOOLCHAIN_ROOT environment variable).
#
# Here are some 3rd party instructions for getting the toolchain:
# 	https://code.google.com/p/kobo-plus/wiki/GettingStarted
#
# Summary instructions adapted for OcherBook:
# 	$ mkdir kobo
# 	$ cd kobo
# 	$ wget https://sourcery.mentor.com/public/gnu_toolchain/arm-none-linux-gnueabi/arm-2010q1-202-arm-none-linux-gnueabi.bin
# 	$ bash ./arm-2010q1*.bin
# Work around the dash vs bash stupidity if so instructed.
# Ensure X is running for a depressing Windows-like install experience.
# You do not need to modify PATH; that is handled by paths below.
# Ideally, install to a standardized location like /opt/toolchains/linux-x86-kobo,
# or make a link, or set the OCHER_TOOLCHAIN_ROOT environment variable like this:
# 	$ export OCHER_TOOLCHAIN_ROOT=/opt/toolchains/linux-x86-kobo
#
# Finally, configure and build with the toolchain:
# 	$ cd OcherBook
# 	$ mkdir build
# 	$ cd build
# 	$ cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchains/kobo.cmake ..
# 	$ make


set(OCHER_TARGET kobo)
set(OCHER_TARGET_KOBO TRUE)

set(WITH_FREETYPE_DEFAULT On)
set(WITH_FRAMEBUFFER_DEFAULT On)

set(CMAKE_SYSTEM_NAME Linux)

if("$ENV{OCHER_TOOLCHAIN_ROOT}" STREQUAL "")
	set(OCHER_TOOLCHAIN_ROOT "/opt/toolchains/linux-x86-kobo/")
else()
	set(OCHER_TOOLCHAIN_ROOT "$ENV{OCHER_TOOLCHAIN_ROOT}")
endif()
message("Using kobo toolchain in ${OCHER_TOOLCHAIN_ROOT}")
set(CMAKE_C_COMPILER      "${OCHER_TOOLCHAIN_ROOT}/bin/arm-none-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER    "${OCHER_TOOLCHAIN_ROOT}/bin/arm-none-linux-gnueabi-g++")
set(CMAKE_FIND_ROOT_PATH  "${OCHER_TOOLCHAIN_ROOT}/arm-none-linux-gnueabi/")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Disable gcc 4.4's warning "note: the mangling of 'va_list' has changed in GCC 4.4"
set(TOOLCHAIN_C_FLAGS "-Wno-psabi")
set(TOOLCHAIN_CXX_FLAGS "-Wno-psabi")
