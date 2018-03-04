set(CMAKE_CXX_STANDARD 11)

if(OCHER_TARGET_SDL)
	find_package(SDL REQUIRED)
	include_directories(${SDL_INCLUDE_DIR})
endif()

list(APPEND OCHER_DEFS __USE_GNU)

string(REPLACE "." ";" OCHER_VERSIONS ${PROJECT_VERSION})
list(GET OCHER_VERSIONS 0 OCHER_MAJOR)
list(GET OCHER_VERSIONS 1 OCHER_MINOR)
list(GET OCHER_VERSIONS 2 OCHER_PATCH)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/version.h
	)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/..
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
	fmt/minizip/unzip.c
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
	util/intrinsics.h
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
	ux/Activity.cpp
	ux/Controller.cpp
	ux/Event.cpp
	ux/Pagination.cpp
	ux/PowerSaver.cpp
	ux/Renderer.cpp
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
	ux/fd/ActivityFd.h
	ux/fd/LibraryActivityFd.cpp
	ux/fd/RendererFd.cpp
	ux/fd/RendererFd.h
	ux/fd/UxControllerFd.cpp
	ux/fd/UxControllerFd.h
	)
if(OCHER_UI_CDK)
	list(APPEND LIB_SOURCES
		ux/cdk/ActivityCdk.h
		ux/cdk/LibraryActivityCdk.cpp
		ux/cdk/LibraryActivityCdk.h
		ux/cdk/RendererCdk.cpp
		ux/cdk/RendererCdk.h
		ux/cdk/UxControllerCdk.cpp
		ux/cdk/UxControllerCdk.h
		)
	list(APPEND OCHER_DEFS UX_CDK)
endif()
if(OCHER_KOBO)
	list(APPEND LIB_SOURCES
		ux/fb/mx50/FrameBufferMx50.cpp
		device/kobo/KoboEvents.cpp
		)
	list(APPEND OCHER_DEFS UX_FB UX_FB_MX50)
endif()
if(OCHER_TARGET_SDL)
	list(APPEND LIB_SOURCES
		ux/fb/sdl/FrameBufferSdl.cpp
		ux/fb/sdl/SdlThread.cpp
		)
	list(APPEND OCHER_DEFS UX_FB UX_FB_SDL)
endif()
if(OCHER_UI_FD)
	list(APPEND OCHER_DEFS UX_FD)
endif()

if(OCHER_FMT_EPUB)
	list(APPEND LIB_SOURCES
		fmt/epub/LayoutEpub.cpp
		fmt/epub/UnzipCache.cpp
		fmt/epub/Epub.cpp
		)
	list(APPEND OCHER_DEFS OCHER_EPUB)
endif()
if(OCHER_FMT_TEXT)
	list(APPEND LIB_SOURCES
		fmt/text/Text.cpp
		fmt/text/LayoutText.cpp
		)
	list(APPEND OCHER_DEFS OCHER_TEXT)
endif()
if(OCHER_FMT_JPEG)
	list(APPEND LIB_SOURCES
		ux/fb/JPEG.cpp
		)
	list(APPEND OCHER_DEFS OCHER_JPEG)
endif()

add_library(ocherbook STATIC
	${LIB_SOURCES}
	)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	list(APPEND OCHER_DEFS LOG_LEVEL=5)
else()
	list(APPEND OCHER_DEFS LOG_LEVEL=2)
endif()

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
if(FALSE AND OCHER_FMT_EPUB)
	target_link_libraries(ocherbook
		hubbub
		#css
		parserutils
		wapcaplet
		)
endif()
if(OCHER_TARGET_SDL)
	target_link_libraries(ocherbook
		${SDL_LIBRARY}
		)
endif()
if(OCHER_UI_CDK)
	target_link_libraries(ocherbook
		cdk
		ncurses
		)
endif()

add_executable(ocher
	ocher.cpp
	)
target_link_libraries(ocher
	ocherbook
	)

add_subdirectory(test)
