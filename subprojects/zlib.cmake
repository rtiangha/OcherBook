include(ExternalProject)

if (WITH_SYSTEM_ZLIB)
    find_library(zlib REQUIRED)
else()
    ExternalProject_add(zlib-external
        URL http://zlib.net/zlib-1.2.12.tar.gz
        URL_HASH SHA256=91844808532e5ce316b3c010929493c0244f3d37593afd6de04f71821d5136d9
        DOWNLOAD_DIR ${TOP_SRCDIR}/subprojects/packagecache/
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --static --prefix=<INSTALL_DIR>
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
        )
    add_library(zlib INTERFACE)
    target_link_libraries(zlib INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/lib/libz.a
        )
    target_include_directories(zlib INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/src/zlib-external/contrib/minizip/
        ${CMAKE_CURRENT_BINARY_DIR}/zlib-external-prefix/include
        )
    add_dependencies(zlib zlib-external)
endif()
