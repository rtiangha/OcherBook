ocher_sources = [
    'Container.cpp',
    'device/Battery.cpp',
    'device/Device.cpp',
    'device/Filesystem.cpp',
    'fmt/Format.cpp',
    'fmt/Layout.cpp',
    'fmt/minizip/ioapi.c',
    'fmt/minizip/ioapi.h',
    'fmt/minizip/unzip.c',
    'fmt/minizip/unzip.h',
    'fmt/text/LayoutText.cpp',
    'fmt/text/LayoutText.h',
    'fmt/text/Text.cpp',
    'fmt/text/Text.h',
    'resources/Bitmaps.cpp',
    'settings/Settings.cpp',
    'shelf/Meta.cpp',
    'shelf/Shelf.cpp',
    'util/Bitops.h',
    'util/Debug.cpp',
    'util/Debug.h',
    'util/DirIter.cpp',
    'util/DirIter.h',
    'util/File.cpp',
    'util/File.h',
    'util/LogAppenderFile.cpp',
    'util/LogAppenderFile.h',
    'util/LogAppenders.cpp',
    'util/LogAppenders.h',
    'util/Logger.cpp',
    'util/Logger.h',
    'util/Path.cpp',
    'util/Path.h',
    'util/Stopwatch.h',
    'util/StrUtil.cpp',
    'util/StrUtil.h',
    'util/intrinsics.h',
    'util/stdex.h',
    'util/time.cpp',
    'util/time.h',
    'ux/Activity.cpp',
    'ux/Activity.h',
    'ux/Controller.cpp',
    'ux/Controller.h',
    'ux/Event.cpp',
    'ux/Event.h',
    'ux/Pagination.cpp',
    'ux/Pagination.h',
    'ux/PowerSaver.cpp',
    'ux/PowerSaver.h',
    'ux/Renderer.cpp',
    'ux/Renderer.h',
    'ux/Types.h',
]
if conf_data.get('OCHER_TARGET_KOBO', false)
    ocher_sources += [
        'device/kobo/KoboEvents.cpp',
        'device/kobo/KoboEvents.h',
        'device/ntx.cpp',
        'device/ntx.h',
    ]
endif
if conf_data.get('UX_FB_LINUX', false)
    ocher_sources += [
        'ux/fb/FbLinux.cpp',
        'ux/fb/FbLinux.h',
    ]
endif
if conf_data.get('UX_FB_MX50', false)
    ocher_sources += [
        'ux/fb/mx50/FrameBufferMx50.cpp',
        'ux/fb/mx50/FrameBufferMx50.h',
    ]
endif
if conf_data.get('UX_FD', false)
    ocher_sources += [
        'ux/fd/ActivityFd.h',
        'ux/fd/LibraryActivityFd.cpp',
        'ux/fd/RendererFd.cpp',
        'ux/fd/RendererFd.h',
        'ux/fd/UxControllerFd.cpp',
        'ux/fd/UxControllerFd.h',
    ]
endif
if conf_data.get('UX_CDK', false)
    ocher_sources += [
        'ux/cdk/ActivityCdk.h',
        'ux/cdk/HomeActivityCdk.h',
        'ux/cdk/LibraryActivityCdk.cpp',
        'ux/cdk/LibraryActivityCdk.h',
        'ux/cdk/RendererCdk.cpp',
        'ux/cdk/RendererCdk.h',
        'ux/cdk/UxControllerCdk.cpp',
        'ux/cdk/UxControllerCdk.h',
    ]
endif
if conf_data.get('UX_FB', false)
    ocher_sources += [
        'ux/fb/BatteryIcon.cpp',
        'ux/fb/bitmap.cpp',
        'ux/fb/BootActivityFb.cpp',
        'ux/fb/ClockIcon.cpp',
        'ux/fb/fontcontext.cpp',
        'ux/fb/FontEngine.cpp',
        'ux/fb/FrameBuffer.cpp',
        'ux/fb/HomeActivityFb.cpp',
        'ux/fb/JPEG.cpp',
        'ux/fb/LibraryActivityFb.cpp',
        'ux/fb/NavBar.cpp',
        'ux/fb/ReadActivityFb.cpp',
        'ux/fb/RendererFb.cpp',
        'ux/fb/SettingsActivityFb.cpp',
        'ux/fb/SleepActivityFb.cpp',
        'ux/fb/SyncActivityFb.cpp',
        'ux/fb/SystemBar.cpp',
        'ux/fb/UxControllerFb.cpp',
        'ux/fb/Widgets.cpp',
    ]
endif
if conf_data.get('UX_FB_SDL', false)
    ocher_sources += [
        'ux/fb/sdl/FrameBufferSdl.cpp',
        'ux/fb/sdl/FrameBufferSdl.h',
        'ux/fb/sdl/SdlThread.cpp',
        'ux/fb/sdl/SdlThread.h',
    ]
endif
if conf_data.get('FMT_EPUB', false)
    ocher_sources += [
        'fmt/epub/LayoutEpub.cpp',
        'fmt/epub/UnzipCache.cpp',
        'fmt/epub/Epub.cpp',
    ]
endif
if conf_data.get('FMT_JPEG', false)
    ocher_sources += [
        'ux/fb/JPEG.cpp',
    ]
endif


version = meson.project_version()
ver_arr = version.split('.')
conf_data.set_quoted('OCHER_VERSION', version)
conf_data.set('OCHER_MAJOR', ver_arr[0])
conf_data.set('OCHER_MINOR', ver_arr[1])
conf_data.set('OCHER_PATCH', ver_arr[2])
configure_file(input : 'meson_config.h.in',
               output : 'ocher_config.h',
               configuration : conf_data)
# TODO  How best to make linux kernel headers available?  KoboLabs has them in their repo.
add_project_arguments(['-I../ocher/linux-2.6.35.3/include'], language : 'cpp')

ocherbook_inc = include_directories('.')
ocherbook = static_library('ocherbook', ocher_sources,
    dependencies : [
        thread_dep,
        zlib_dep,
        Signals_dep,
        json_dep,
        libev_dep,
        freetype_dep,
        sdl_dep,
        mxml_dep,
    ],
)

ocherbook_dep = declare_dependency(
    include_directories : ocherbook_inc,
    link_with: ocherbook,
    dependencies : [
        thread_dep,
        zlib_dep,
        Signals_dep,
        json_dep,
        libev_dep,
        freetype_dep,
        sdl_dep,
        mxml_dep,
    ],
)

ocher = executable('ocher', ['ocher.cpp', 'ocher.h'],
    dependencies : [ocherbook_dep],
)

subdir('test')
