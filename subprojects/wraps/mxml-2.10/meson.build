project('mxml', 'c',
    version : '2.10',
    license : 'LGPL2'
)

src = files([
    'mxml-attr.c',
    'mxml-entity.c',
    'mxml-file.c',
    'mxml-get.c',
    'mxml-index.c',
    'mxml-node.c',
    'mxml-private.c',
    'mxml-search.c',
    'mxml-set.c',
    'mxml-string.c',
])

conf = configuration_data()
conf.set('VERSION', meson.project_version())
conf.set('HAVE_LONG_LONG', 1)
conf.set('HAVE_SNPRINTF', 1)
conf.set('HAVE_VSNPRINTF', 1)
conf.set('HAVE_STRDUP', 1)
conf.set('HAVE_PTHREAD_H', 1)
configure_file(
    input : 'meson.config.h.in',
    output : 'config.h',
    configuration : conf,
)

thread_dep = dependency('threads')

mxml = static_library('mxml', src,
    dependencies : thread_dep,
)

incdir = include_directories('.')

mxml_dep = declare_dependency(
    link_with : mxml,
    include_directories : incdir,
)
