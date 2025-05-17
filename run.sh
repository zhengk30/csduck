#!/bin/bash

rm -rf build
meson setup build
meson compile -C build

if [ "$1" = server ]; then
    build/server
elif [ "$1" = client ]; then
    build/client
else
    echo "usage: ./run.sh server | client"
fi
