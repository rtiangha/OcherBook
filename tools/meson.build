cc = meson.get_compiler('c')
dl_dep = cc.find_library('dl', required : true)

link_args = []
if meson.is_cross_build()
    link_args = ['-static']
endif

ioctltrace = shared_library('ioctltrace', ['ioctltrace.c'],
    dependencies: [dl_dep],
)

fbctl = executable('fbctl', ['fbctl.cpp'],
    dependencies : [ocherbook_dep],
    link_args : link_args,
)
