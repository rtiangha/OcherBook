include(ExternalProject)

if (WITH_SYSTEM_LIBEV)
    find_library(ev REQUIRED)
else()
    ExternalProject_add(libev-external
        URL http://dist.schmorp.de/libev/Attic/libev-4.33.tar.gz
        URL_HASH SHA256=507eb7b8d1015fbec5b935f34ebed15bf346bed04a11ab82b8eee848c4205aea
        DOWNLOAD_DIR ${TOP_SRCDIR}/subprojects/packagecache/
        CONFIGURE_COMMAND ${cross_env_prefix} <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
        )
    add_library(libev INTERFACE)
    target_link_libraries(libev INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/libev-external-prefix/lib/libev.a)
    target_include_directories(libev INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/libev-external-prefix/include)
    add_dependencies(libev libev-external)
endif()
