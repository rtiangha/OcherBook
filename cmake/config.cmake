# Target(s) set by toolchain file:
# OCHER_TARGET=foo-submodel
# OCHER_TARGET_FOO
# OCHER_TARGET_FOO_SUBMODEL
#
# User-settable:
# WITH_FOO				Enable feature FOO
# WITH_SYSTEM_FOO		If FOO is needed use system's FOO (vs build from source)
# WITHOUT_FOO			Disable feature FOO
#
# Internal:
# WITH_FOO_DEFAULT		Default to enable feature FOO (if applicable)
# WITH_SYSTEM_FOO_DEFAULT	Default to use system's FOO vs build (if applicable)

if (NOT OCHER_TARGET AND NOT CMAKE_CROSSCOMPILING)
	# Some guesses...
	set(OCHER_TARGET sdl)
	set(OCHER_TARGET_SDL TRUE)
endif()

### Debugging
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(OCHER_AIRBAG_FD_DEF TRUE)
else()
	set(OCHER_AIRBAG_FD_DEF FALSE)
endif()
set(OCHER_AIRBAG_FD ${OCHER_AIRBAG_FD_DEF} CACHE BOOL "Use airbagfd crash handler (only for unix)")

### eBook formats
set(OCHER_FMT_EPUB TRUE CACHE BOOL "Support EPUB books")
set(OCHER_FMT_TEXT TRUE CACHE BOOL "Support plain text books")
set(OCHER_FMT_HTML TRUE CACHE BOOL "Support HTML books")
set(OCHER_FMT_JPEG TRUE CACHE BOOL "Support JPEG images")
set(OCHER_FMT_PNG  TRUE CACHE BOOL "Support PNG images")

if(OCHER_FMT_EPUB)
	set(WITH_ZLIB TRUE)
endif()
if(OCHER_FMT_EPUB OR OCHER_FMT_HTML)
	set(WITH_MXML TRUE)
endif()

### Output devices
set(OCHER_UI_FD TRUE CACHE BOOL "File descriptor user interface")
set(OCHER_UI_CDK FALSE CACHE BOOL "CDK (ncurses) based user interface")
