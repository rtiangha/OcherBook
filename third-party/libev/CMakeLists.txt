include(ExternalProject)

option(WITH_SYSTEM_LIBEV "Use installed libev?  (Else build from source)" ${WITH_SYSTEM_FREETYPE_DEFAULT})

if (WITH_SYSTEM_LIBEV)
	find_library(ev REQUIRED)
else()
	ExternalProject_add(libev-external
		URL http://dist.schmorp.de/libev/Attic/libev-4.15.tar.gz
		CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
		BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
		)
	add_library(libev INTERFACE)
	target_link_libraries(libev INTERFACE
		${CMAKE_CURRENT_BINARY_DIR}/libev-external-prefix/lib/libev.a)
	target_include_directories(libev INTERFACE
		${CMAKE_CURRENT_BINARY_DIR}/libev-external-prefix/include)
	add_dependencies(libev libev-external)
endif()
