# TODO get_propery INCLUDE_DIRECTORIES of various third_party dirs

include_directories(
	${ROOT_DIR}
	${ZLIB_INCLUDE_DIR}
	${LIBEV_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIR}
	${THIRD_PARTY_DIR}
	.
	)
if(OCHER_KOBO)
	include_directories(${CMAKE_SOURCE_DIR}/linux)
endif()
if(OCHER_SDL OR OCHER_UI_CDK)
	include_directories(/usr/local/include)
	link_directories(/usr/local/lib)
endif()
if(OCHER_FMT_EPUB)
	include_directories(
		${HUBBUB_INC_DIR}
		${LIBCSS_INC_DIR}
		${WAPCAPLET_INC_DIR}
		${PARSERUTILS_INC_DIR}
		${MXML_INCLUDE_DIR}
		)
endif()

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
	util/Intrinsics.h
	util/Lock.cpp
	util/Lock.h
	util/LogAppenderFile.cpp
	util/LogAppenderFile.h
	util/LogAppenders.cpp
	util/LogAppenders.h
	util/Logger.cpp
	util/Logger.h
	util/Monitor.cpp
	util/Monitor.h
	util/MurmurHash2.cpp
	util/MurmurHash2.h
	util/Path.cpp
	util/Path.h
	util/RWLock.cpp
	util/RWLock.h
	util/Random.cpp
	util/Random.h
	util/Stopwatch.h
	util/StrUtil.cpp
	util/StrUtil.h
	util/Thread.cpp
	util/Thread.h
	util/ThreadLocal.h
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
if(OCHER_SDL)
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
		#fmt/epub/Css.cpp
		#fmt/epub/ParseHtml.c
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
if(OCHER_SDL)
	target_link_libraries(ocherbook
		SDL
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


include(ProcessorCount)
ProcessorCount(NUM_CPUS)
if(NOT NUM_CPUS EQUAL 0)
	set(CPPCHECK_CPUS -j${NUM_CPUS})
endif()
get_target_property(OCHER_INCS ocherbook INCLUDE_DIRECTORIES)
foreach(INC ${OCHER_INCS})
	list(APPEND CPPCHECK_INCS "-I${INC}")
endforeach()
add_custom_target(cppcheck
	COMMAND cppcheck
		--max-configs=100 ${CPPCHECK_CPUS}
		-i${THIRD_PARTY_DIR} --config-exclude=${THIRD_PARTY_DIR}
		--std=c++11 --std=posix --language=c++
		--enable=warning
		${CPPCHECK_INCS}
		${CMAKE_CURRENT_SOURCE_DIR} 2> ${CMAKE_CURRENT_BINARY_DIR}/cppcheck.log
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	)


configure_file(${PROJECT_SOURCE_DIR}/doc/Doxyfile
	${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

add_custom_target(doxygen
	COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	)

add_subdirectory(test)
