include(ExternalProject)

option(WITH_FREETYPE "Use Freetype for font rendering" ON)
option(WITH_SYSTEM_FREETYPE "Use installed FreeType?  (Else build from source)" OFF)

if (WITH_FREETYPE)

if (WITH_SYSTEM_FREETYPE)
	find_library(freetype REQUIRED)
else()
	ExternalProject_add(freetype-external
		URL http://download.savannah.gnu.org/releases/freetype/freetype-2.4.12.tar.bz2
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-shared --without-bzip2
		BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
		INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install
		)
	add_library(freetype INTERFACE)
	target_link_libraries(freetype INTERFACE
		${CMAKE_CURRENT_BINARY_DIR}/freetype-external-prefix/lib/libfreetype.a
		)
	target_include_directories(freetype INTERFACE
		${CMAKE_CURRENT_BINARY_DIR}/freetype-external-prefix/include/freetype2
		${CMAKE_CURRENT_BINARY_DIR}/freetype-external-prefix/include
		)
	add_dependencies(freetype freetype-external)
endif()

endif()
