set(CMAKE_CXX_STANDARD 11)

list(APPEND OCHER_DEFS __USE_GNU)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

list(APPEND LIB_SOURCES
    Container.cpp
    device/Battery.cpp
    device/Device.cpp
    device/Filesystem.cpp
    fmt/Format.cpp
    fmt/Layout.cpp
    fmt/minizip/ioapi.c
    fmt/minizip/ioapi.h
    fmt/minizip/unzip.c
    fmt/minizip/unzip.h
    fmt/text/LayoutText.cpp
    fmt/text/LayoutText.h
    fmt/text/Text.cpp
    fmt/text/Text.h
    resources/Bitmaps.cpp
    settings/Settings.cpp
    shelf/Meta.cpp
    shelf/Shelf.cpp
    util/Bitops.h
    util/Buffer.cpp
    util/Buffer.h
    util/Clock.cpp
    util/Clock.h
    util/Debug.cpp
    util/Debug.h
    util/DirIter.cpp
    util/DirIter.h
    util/File.cpp
    util/File.h
    util/LogAppenderFile.cpp
    util/LogAppenderFile.h
    util/LogAppenders.cpp
    util/LogAppenders.h
    util/Logger.cpp
    util/Logger.h
    util/Path.cpp
    util/Path.h
    util/Stopwatch.h
    util/StrUtil.cpp
    util/StrUtil.h
    util/intrinsics.h
    ux/Activity.cpp
    ux/Controller.cpp
    ux/Event.cpp
    ux/Pagination.cpp
    ux/PowerSaver.cpp
    ux/Renderer.cpp
)
if(UX_FD)
    list(APPEND LIB_SOURCES
        ux/fd/ActivityFd.h
        ux/fd/LibraryActivityFd.cpp
        ux/fd/RendererFd.cpp
        ux/fd/RendererFd.h
        ux/fd/UxControllerFd.cpp
        ux/fd/UxControllerFd.h
    )
endif()
if(UX_CDK)
    list(APPEND LIB_SOURCES
        ux/cdk/ActivityCdk.h
        ux/cdk/HomeActivityCdk.h
        ux/cdk/LibraryActivityCdk.cpp
        ux/cdk/LibraryActivityCdk.h
        ux/cdk/RendererCdk.cpp
        ux/cdk/RendererCdk.h
        ux/cdk/UxControllerCdk.cpp
        ux/cdk/UxControllerCdk.h
    )
endif()
if(UX_FB)
    list(APPEND LIB_SOURCES
        ux/fb/BatteryIcon.cpp
        ux/fb/BootActivityFb.cpp
        ux/fb/ClockIcon.cpp
        ux/fb/FontEngine.cpp
        ux/fb/FrameBuffer.cpp
        ux/fb/FreeType.cpp
        ux/fb/HomeActivityFb.cpp
        ux/fb/JPEG.cpp
        ux/fb/LibraryActivityFb.cpp
        ux/fb/NavBar.cpp
        ux/fb/ReadActivityFb.cpp
        ux/fb/RendererFb.cpp
        ux/fb/RleBitmap.cpp
        ux/fb/SettingsActivityFb.cpp
        ux/fb/SleepActivityFb.cpp
        ux/fb/SyncActivityFb.cpp
        ux/fb/SystemBar.cpp
        ux/fb/UxControllerFb.cpp
        ux/fb/Widgets.cpp
    )
endif()
if(OCHER_KOBO)
    list(APPEND LIB_SOURCES
        ux/fb/mx50/FrameBufferMx50.cpp
        device/kobo/KoboEvents.cpp
    )
    set(UX_FB_MX50 TRUE)
endif()
if(UX_FB_SDL)
    include_directories(${SDL_INCLUDE_DIR})
    list(APPEND LIB_SOURCES
        ux/fb/sdl/FrameBufferSdl.cpp
        ux/fb/sdl/SdlThread.cpp
    )
endif()

if(FMT_EPUB)
    list(APPEND LIB_SOURCES
        fmt/epub/LayoutEpub.cpp
        fmt/epub/UnzipCache.cpp
        fmt/epub/Epub.cpp
    )
endif()
if(FMT_JPEG)
    list(APPEND LIB_SOURCES
        ux/fb/JPEG.cpp
    )
endif()

string(REPLACE "." ";" OCHER_VERSIONS ${PROJECT_VERSION})
list(GET OCHER_VERSIONS 0 OCHER_MAJOR)
list(GET OCHER_VERSIONS 1 OCHER_MINOR)
list(GET OCHER_VERSIONS 2 OCHER_PATCH)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake_config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/ocher_config.h
)
add_definitions(-include ocher_config.h)

add_library(ocherbook STATIC
    ${LIB_SOURCES}
)

target_compile_definitions(ocherbook
    PRIVATE
        ${OCHER_DEFS}
)

target_link_libraries(ocherbook
    Signals
    json
    freetype
    libev
    zlib
    mxml
    pthread
    bz2  # TODO freetype (may!) want this
)
if(SDL)
    target_link_libraries(ocherbook ${SDL_LIBRARY})
endif()
if(UX_CDK)
    target_link_libraries(ocherbook cdk ncurses)
endif()
if(AIRBAG_FD)
    target_link_libraries(ocherbook airbag_fd)
endif()

add_executable(ocher ocher.cpp)
target_link_libraries(ocher ocherbook)

add_subdirectory(test)
