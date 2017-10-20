option(ZLIB_USE_SYSTEM "Use the system's libz.so?")

include(ExternalProject)

if (ZLIB_USE_SYSTEM)
	find_library(zlib REQUIRED)
else()
	ExternalProject_add(zlib-external
		URL http://zlib.net/zlib-1.2.11.tar.gz
		URL_HASH SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1
		BUILD_IN_SOURCE 1
		#CFLAGS="$(CFLAGS_COMMON) -O3" CC=$(CC)
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --static --prefix=<INSTALL_DIR>
		BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
		)
	list(APPEND ZLIB_INCLUDE_DIR
		${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/src/zlib-external/contrib/minizip/
		${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/include)
	set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIR} PARENT_SCOPE)
	add_library(zlib STATIC IMPORTED GLOBAL)
	set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/lib/libz.a)
	add_dependencies(zlib zlib-external)
endif()

