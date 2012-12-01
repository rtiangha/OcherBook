#!/bin/sh

[ -z "$OCHER_IP" ] && OCHER_IP=192.168.1.68

(
    sleep 1
    echo "root"
    echo "cd /tmp"
    echo "lcd build"
    echo "put ocher"
    echo "lcd .."
    echo "put DejaVuSerif.ttf"
    echo "put DejaVuSerif-Italic.ttf"
    echo "put DejaVuSerif-Bold.ttf"
    echo "put DejaVuSerif-BoldItalic.ttf"
    echo "quit"
) | ftp $OCHER_IP

(
    echo "root"
    sleep 1
    echo "cd /tmp"
    echo "chmod +x ocher"
    #echo "killall nickel"
    #echo "killall ocher"
    #echo "nohup ./ocher -vv &"
) | telnet $OCHER_IP
