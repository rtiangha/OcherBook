include(ExternalProject)

if (WITH_SYSTEM_MXML)
    find_library(mxml REQUIRED)
else()
    ExternalProject_add(mxml-external
        URL https://github.com/michaelrsweet/mxml/releases/download/v2.12/mxml-2.12.tar.gz 
        URL_HASH SHA256=6bfb53baa1176e916855bd3b6d592fd5b962c3c259aacdb5670d90c57ce4034f
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
