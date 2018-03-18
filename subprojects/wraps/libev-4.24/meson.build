project('libev', 'c',
    version : '4.24',
    license : 'BSD'
)

src = files([
    'ev.c',
    'event.c',
])

conf = configuration_data()
conf.set_quoted('VERSION', meson.project_version())
if host_machine.system() == 'linux'
    conf.set('HAVE_EPOLL_CTL', 1)
    conf.set('HAVE_EVENTFD', 1)
    conf.set('HAVE_INOTIFY_INIT', 1)
    conf.set('HAVE_SIGNALFD', 1)
    conf.set('HAVE_SYS_EPOLL_H', 1)
    conf.set('HAVE_SYS_INOTIFY_H', 1)
    conf.set('HAVE_SYS_SIGNALFD_H', 1)
endif
if host_machine.system() == 'freebsd'
    conf.set('HAVE_KQUEUE', 1)
endif
conf.set('HAVE_CLOCK_GETTIME', 1)
conf.set('HAVE_CLOCK_SYSCALL', 1)
conf.set('HAVE_FLOOR', 1)
conf.set('HAVE_NANOSLEEP', 1)
conf.set('HAVE_POLL', 1)
conf.set('HAVE_POLL_H', 1)
conf.set('HAVE_SELECT', 1)
conf.set('HAVE_SYS_EVENT_H', 1)
conf.set('HAVE_SYS_SELECT_H', 1)
configure_file(output : 'config.h',
               configuration : conf)

cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

libev = static_library('libev', src,
    c_args : ['-Wno-unused-value', '-Wno-unused-function'],
    dependencies : m_dep
)

incdir = include_directories('.')

libev_dep = declare_dependency(
    link_with : libev,
    include_directories : incdir
)
