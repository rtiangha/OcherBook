# Quick walk-through for Kobo

Currently, OcherBook is not a full replacement for the Kobo's firmware.  It is
only intended to replace "nickel".  The Kobo's kernel, boot scripts, and
wireless networking are left as-is.

You'll want to enable FTP and telnet access to your Kobo. Notes on rooting your
Kobo: http://uscoffings.net/clc/tech/embedded/kobo/

Optional, but convenient, is to also add a static IP mapping for you Kobo in
your router, and block its access to the `*.kobo.com` domains or even restrict
its internet access in general so it cannot update overtop your work.

Then build and upload "ocher" and an epub, telnet in, and run it.  Run with
"-vvv" to get some logging output to stderr.  The script `tools/put-kobo.sh` can
help here.

So a full session, assuming your Kobo has ftp and telnet enabled, might be:

```
$ export KOBO_IP=192.168.1.128
$ cd OcherBook
$ meson build-kobo --cross-file toolchains/kobo-linaro.meson -Dkobo=true
$ cd build-kobo
$ ninja
$ ../tools/put-kobo.sh
```
