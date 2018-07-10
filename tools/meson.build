cc = meson.get_compiler('c')
dl_dep = cc.find_library('dl', required : true)

ioctltrace = shared_library('ioctltrace', ['ioctltrace.c'],
    dependencies: [dl_dep],
)

fbctl = executable('fbctl', ['fbctl.cpp'],
    dependencies : [ocherbook_dep],
)
