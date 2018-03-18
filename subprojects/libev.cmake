include(ExternalProject)

if (WITH_SYSTEM_LIBEV)
    find_library(ev REQUIRED)
else()
    ExternalProject_add(libev-external
        URL http://dist.schmorp.de/libev/Attic/libev-4.24.tar.gz
        URL_HASH SHA256=973593d3479abdf657674a55afe5f78624b0e440614e2b8cb3a07f16d4d7f821
        DOWNLOAD_DIR ${TOP_SRCDIR}/subprojects/packagecache/
        CONFIGURE_COMMAND CC=${CMAKE_C_COMPILER} <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
        )
    add_library(libev INTERFACE)
    target_link_libraries(libev INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/libev-external-prefix/lib/libev.a)
    target_include_directories(libev INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/libev-external-prefix/include)
    add_dependencies(libev libev-external)
endif()
