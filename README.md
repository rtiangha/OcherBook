# OcherBook

![Ocher handprint](doc/hand.png)

OcherBook is an open-source ebook reader, supporting multiple file formats and
output devices.  The major goals are:

- Return control and privacy of the reading experience to the reader
- Portable
- Modular design
- Small with few external dependencies.

## Target Devices

Perhaps the largest distinguishing implementation detail of OcherBook is that
its in-memory format is not DOM, and the rendering backends are not web
browsers.  Think DVI.  Therefore it is not going to render complex books nearly
as accurately, but it is far more portable and efficient.  This may eventually
turn out to be a mistake, but for now it's an advantage that I'm running with.

It also handles its own event loop and its own rendering, so it could
potentially run on _very_ limited devices.  This is, perhaps, the singular charm
of this project.

At the moment it's mostly tested on Kobo and SDL.

## Building

Building is supported with Meson (native and cross-compile) and CMake
(currently only native).

To get started, try something simple like:

    $ cd OcherBook
    $ meson builddir
    $ cd builddir
    $ ninja

Check the Wiki for more detailed instructions.

To cross-compile, look in `toolchains` for a toolchain file for your
device.  Each toolchain file should include some notes regarding how to acquire
and install the cross-compiling toolchain, and how to configure OcherBook for it.

Many features are configurable.  Some feature libraries can be disabled, or
enabled to build against the system's libraries, or enabled to download and
build from source (perhaps useful when cross-compiling).  For details, see
`meson_options.txt` or `cmake_options.cmake`.

## License

OcherBook itself is GPLv3.  See `COPYING`.

See `doc/LICENSES` for all relevant licenses.
