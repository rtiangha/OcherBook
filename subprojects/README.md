Currently not using git subprojects; may in the future.

If the subproject is very simple (eg a header-only library), make a local copy
of the header and write a native meson.build.  Wrap more complex progjects.


catch
doctest
    copy header
Signals
    copy header
json
    copy header
airbag_fd
    native meson
zlib
    wrap
libev
    wrap (local)
mxml
freetype
sdl
cdk
