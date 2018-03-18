include(ExternalProject)

if (WITH_SYSTEM_MXML)
    find_library(mxml REQUIRED)
else()
    ExternalProject_add(mxml-external
        URL https://github.com/michaelrsweet/mxml/releases/download/release-2.10/mxml-2.10.tar.gz
        URL_MD5 8804c961a24500a95690ef287d150abe
        DOWNLOAD_DIR ${TOP_SRCDIR}/subprojects/packagecache/
        BUILD_IN_SOURCE 1
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} libmxml.a
        )
    add_library(mxml INTERFACE)
    target_link_libraries(mxml INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/mxml-external-prefix/lib/libmxml.a
        )
    target_include_directories(mxml INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/mxml-external-prefix/include
        )
    add_dependencies(mxml mxml-external)
endif()
