option(ZLIB_USE_SYSTEM "Use the system's libz.so?")

include(ExternalProject)

if (ZLIB_USE_SYSTEM)
	find_library(zlib REQUIRED)
else()
	ExternalProject_add(zlib-external
		URL http://zlib.net/zlib-1.2.8.tar.gz
		URL_MD5 44d667c142d7cda120332623eab69f40
		BUILD_IN_SOURCE 1
		UPDATE_COMMAND ""
		#CFLAGS="$(CFLAGS_COMMON) -O3" CC=$(CC)
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --static --prefix=<INSTALL_DIR>
		BUILD_COMMAND make
		)
	list(APPEND ZLIB_INCLUDE_DIR
		${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/src/zlib-external/contrib/minizip/
		${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/include)
	set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIR} PARENT_SCOPE)
	add_library(zlib STATIC IMPORTED GLOBAL)
	set_property(TARGET zlib PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/lib/libz.a)
	add_dependencies(zlib zlib-external)
endif()

