include(ExternalProject)

if (FREETYPE)

if (WITH_SYSTEM_FREETYPE)
    find_library(freetype REQUIRED)
else()
    ExternalProject_add(freetype-external
        URL https://download.savannah.gnu.org/releases/freetype/freetype-2.10.2.tar.gz
        URL_HASH SHA256=e09aa914e4f7a5d723ac381420949c55c0b90b15744adce5d1406046022186ab
        DOWNLOAD_DIR ${TOP_SRCDIR}/subprojects/packagecache/
        CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-shared --without-bzip2
        BUILD_COMMAND ${CMAKE_MAKE_PROGRAM}
        #CMAKE_ARGS "-G@CMAKE_GENERATOR@" "-DCMAKE_BUILD_TYPE=${configuration}"
        #    "-DCMAKE_CONFIGURATION_TYPES=${configuration}"
        #    "-DCMAKE_INSTALL_PREFIX=@HUNTER_PACKAGE_INSTALL_PREFIX@"
        #    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
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
