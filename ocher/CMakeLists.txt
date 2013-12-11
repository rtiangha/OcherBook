# TODO get_propery INCLUDE_DIRECTORIES of various third_party dirs

include_directories(
	${ROOT_DIR}
	${ROOT_DIR}/clc
	${MXML_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${LIBEV_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIR}
	${THIRD_PARTY_DIR}
	)
if(OCHER_KOBO)
	include_directories(${CMAKE_SOURCE_DIR}/linux)
endif()
if(OCHER_SDL)
	include_directories(/usr/local/include)
	link_directories(/usr/local/lib)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_definitions(-DCLC_LOG_LEVEL=5)
else()
	add_definitions(-DCLC_LOG_LEVEL=2)
endif()

list(APPEND TESTS
	test/OcherTest.cpp
	)

list(APPEND SOURCES
	fmt/Layout.cpp
	fmt/Format.cpp
	fmt/minizip/ioapi.c
	fmt/minizip/unzip.c
	#ux/fd/BrowseFd.cpp
	#ux/fd/RenderFd.cpp
	#ux/fd/FactoryFd.cpp
	ux/Renderer.cpp
	ux/LibraryActivity.cpp
	ux/SettingsActivity.cpp
	ux/BootActivity.cpp
	ux/Activity.cpp
	ux/PowerSaver.cpp
	ux/Pagination.cpp
	ux/fb/RenderFb.cpp
	ux/fb/FreeType.cpp
	ux/fb/Widgets.cpp
	ux/fb/ClockIcon.cpp
	ux/fb/RleBitmap.cpp
	ux/fb/BatteryIcon.cpp
	ux/fb/FactoryFb.cpp
	ux/fb/NavBar.cpp
	ux/fb/FontEngine.cpp
	ux/fb/SystemBar.cpp
	ux/fb/FrameBuffer.cpp
	ux/ReadActivity.cpp
	ux/HomeActivity.cpp
	ux/SyncActivity.cpp
	ux/Event.cpp
	ux/Factory.cpp
	ux/Controller.cpp
	settings/Settings.cpp
	shelf/Shelf.cpp
	shelf/Meta.cpp
	resources/Bitmaps.cpp
	ocher.cpp
	device/Device.cpp
	device/Battery.cpp
	device/Filesystem.cpp
	)
if(OCHER_UI_NCURSES)
	list(APPEND SOURCES
		ux/ncurses/Browse.cpp
		ux/ncurses/FactoryNC.cpp
		ux/ncurses/RenderCurses.cpp
		)
endif()
if(OCHER_KOBO)
	list(APPEND SOURCES
		ux/fb/mx50/fb.cpp
		ux/fb/mx50/FactoryFbMx50.cpp
		device/kobo/KoboEvents.cpp
		)
endif()
if(OCHER_SDL)
	list(APPEND SOURCES
		ux/fb/sdl/FactoryFbSdl.cpp
		ux/fb/sdl/FbSdl.cpp
		ux/fb/sdl/SdlLoop.cpp
		)
endif()
if(OCHER_EPUB)
	list(APPEND SOURCES
		#fmt/epub/Css.cpp
		fmt/epub/LayoutEpub.cpp
		fmt/epub/UnzipCache.cpp
		fmt/epub/Epub.cpp
		)
	add_definitions(-DOCHER_EPUB)
endif()
if(OCHER_TEXT)
	list(APPEND SOURCES
		fmt/text/Text.cpp
		fmt/text/LayoutText.cpp
		)
	add_definitions(-DOCHER_TEXT)
endif()

add_executable(ocher
	${SOURCES}
	)
target_link_libraries(ocher
	freetype
	libev
	zlib
	mxml
	clc
	pthread
	bz2  # TODO freetype (may!) want this
	)
if(OCHER_SDL)
	target_link_libraries(ocher
		SDL
		)
endif()


include(ProcessorCount)
ProcessorCount(NUM_CPUS)
if(NOT NUM_CPUS EQUAL 0)
	set(CPPCHECK_FLAGS -j${NUM_CPUS})
endif()
get_target_property(OCHER_INCS ocher INCLUDE_DIRECTORIES)
foreach(INC ${OCHER_INCS})
	list(APPEND CPPCHECK_INCS "-I${INC}")
endforeach()
add_custom_target(cppcheck
	#COMMAND cppcheck --check-config $(INCS) ocher/ 2> $(BUILD_DIR)/cppcheck.log
	COMMAND cppcheck --max-configs=100 ${CPPCHECK_FLAGS} --enable=all ${CPPCHECK_INCS} ${CMAKE_CURRENT_SOURCE_DIR} 2> ${CMAKE_CURRENT_BINARY_DIR}/cppcheck.log
	)


add_custom_target(doxygen
	COMMAND doxygen ${PROJECT_SOURCE_DIR}/doc/Doxyfile
	)
