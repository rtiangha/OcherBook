option(MXML_USE_SYSTEM "Use the system's libmxml.so?  Recommend OFF for embedded.")

include(ExternalProject)

if (MXML_USE_SYSTEM)
	find_library(mxml REQUIRED)
else()
	ExternalProject_add(mxml-external
		# Note - non-https sometimes only yields a 0-byte file...
		URL https://www.msweet.org/files/project3/mxml-2.8.tar.gz
		URL_MD5 d85ee6d30de053581242c4a86e79a5d2
		BUILD_IN_SOURCE 1
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
		BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} libmxml.a
		)
	set(MXML_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/mxml-external-prefix/include PARENT_SCOPE)
	add_library(mxml STATIC IMPORTED GLOBAL)
	set_property(TARGET mxml PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/mxml-external-prefix/lib/libmxml.a)
	add_dependencies(mxml mxml-external)
endif()
