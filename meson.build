project('ocher', ['cpp', 'c'],
    version: '0.1.0',
    meson_version: '>=0.44.0',
    default_options : 'cpp_std=c++11',
    license: 'GPL3')

if get_option('buildtype').startswith('debug')
    add_project_arguments([
            '-DDEBUG',
            '-DGLIBCXX_ASSERTIONS',
            '-ggdb3',
            '-fasynchronous-unwind-tables',
        ],
        language : 'cpp')
else
    # meson issue #2566 would obsolete this:
    add_project_arguments('-DNDEBUG', language : 'cpp')
endif

compiler = meson.get_compiler('cpp')
warning_flags = [
    '-Wall',
    '-Wextra',
    #'-Wcast-align',                # TODO
    #'-Wcast-align=strict',         # TODO
    #'-Wold-style-cast',            # too many
    #'-Wshadow',                    # libev, json too noisy
    #'-Wuseless-cast',              # libev, json too noisy
    #'-Wzero-as-null-pointer-constant', # json, Delegate too noisy
    '-Wcast-qual',
    '-Wduplicated-cond',
    '-Werror=suggest-override',
    '-Wformat=2',
    '-Wframe-larger-than=16384',
    '-Wlogical-op',
    '-Wno-c++17-compat-mangling',   # libev too noisy
    '-Wno-multichar',
    '-Wno-unused-parameter',
    '-Wnull-dereference',
]
foreach extra_arg : warning_flags
    if compiler.has_argument(extra_arg)
        add_project_arguments([extra_arg], language: 'cpp')
    endif
endforeach

conf_data = configuration_data()
conf_data.set('TOP_SRCDIR', meson.source_root())

# TODO  meson 0.47 https://mesonbuild.com/Build-options.html#features
if get_option('kobo')
    add_project_arguments(['-Os', '-static'], language : 'c')
    add_project_arguments(['-Os', '-static'], language : 'cpp')
    conf_data.set('OCHER_TARGET_KOBO', true)
    # TODO  set hardware per submodel
    conf_data.set('UX_FB', true)
    conf_data.set('UX_FB_LINUX', true)
    conf_data.set('UX_FB_MX50', true)
endif

thread_dep = dependency('threads')

catch_sp = subproject('catch')
catch_dep = catch_sp.get_variable('catch_dep')

Signals_sp = subproject('Signals')
Signals_dep = Signals_sp.get_variable('Signals_dep')

json_sp = subproject('json')
json_dep = json_sp.get_variable('json_dep')

if get_option('airbag_fd')
    airbag_fd_sp = subproject('airbag_fd')
    airbag_fd_dep = airbag_fd_sp.get_variable('airbag_fd_dep')
else
    airbag_fd_dep = disabler()
endif
if airbag_fd_dep.found()
    conf_data.set('AIRBAG_FD', true)
endif

zlib_dep = dependency('zlib', fallback : ['zlib', 'zlib_dep'])

libev_dep = dependency('libev', fallback : ['libev', 'libev_dep'])

mxml_dep = dependency('mxml', fallback : ['mxml', 'mxml_dep'])

freetype_dep = dependency('freetype2', fallback : ['freetype', 'freetype_dep'])

if get_option('ux_fd')
    conf_data.set('UX_FD', true)
endif

if get_option('ux_cdk')
    cdk_dep = dependency('cdk')
else
    cdk_dep = dependency('cdk', required : false)
endif
if cdk_dep.found()
    conf_data.set('UX_CDK', true)
endif

if get_option('ux_fb')
    conf_data.set('UX_FB', true)
    if get_option('sdl')
        sdl_dep = dependency('sdl')
    else
        sdl_dep = dependency('sdl', required : false)
    endif
    if sdl_dep.found()
        conf_data.set('UX_FB_SDL', true)
    endif
endif

if get_option('epub')
    conf_data.set('FMT_EPUB', true)
endif

if get_option('html')
    conf_data.set('FMT_HTML', true)
endif

if get_option('jpeg')
    conf_data.set('FMT_JPEG', true)
endif

if get_option('png')
    conf_data.set('FMT_PNG', true)
endif

subdir('ocher')
subdir('docs')
subdir('tools')
