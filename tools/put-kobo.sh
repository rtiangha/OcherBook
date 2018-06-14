#!/bin/sh

[ -z "$KOBO_IP" ] && KOBO_IP=192.168.1.128

(
    sleep 1
    echo "root"
    echo "cd /usr/local"
    echo "mkdir ocher"
    echo "cd ocher"
    echo "lcd ocher"
    echo "put ocher"
    echo "chmod 0755 ocher"
    echo "quit"
) | ftp $KOBO_IP

(
    sleep 1
    echo "root"
    echo "cd /usr/local/ocher"
    echo "killall ocher"
    echo "> nohup.out"
    echo "nohup ./ocher -vvv"
    echo "tail -f nohup.out"
    while true; do sleep 10 ; done
) | telnet $KOBO_IP
