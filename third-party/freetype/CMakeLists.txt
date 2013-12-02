option(FREETYPE_USE_SYSTEM "Use the system's libmfreetype.so?  Recommend OFF for embedded.")

include(ExternalProject)

if (FREETYPE_USE_SYSTEM)
	find_library(freetype REQUIRED)
else()
	ExternalProject_add(freetype-external
		URL http://download.savannah.gnu.org/releases/freetype/freetype-2.4.12.tar.bz2
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-shared --without-bzip2
		BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
		INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
		)
	set(FREETYPE_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/freetype-external-prefix/include/freetype2
		${CMAKE_CURRENT_BINARY_DIR}/freetype-external-prefix/include PARENT_SCOPE)
	add_library(freetype STATIC IMPORTED GLOBAL)
	set_property(TARGET freetype PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/freetype-external-prefix/lib/libfreetype.a)
	add_dependencies(freetype freetype-external)
endif()


