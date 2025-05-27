#!/bin/bash

## Build DuckDB's source
cd third-party/duckdb-source
make
cd ../..

## Build client and server
rm -rf build
meson setup build
meson compile -C build

if [ "$1" = server ]; then
    if [ "$#" -ne 2 ]; then
        build/server
    else
        build/server "$2"
    fi
elif [ "$1" = client ]; then
    build/client
else
    echo "usage: ./run.sh server | client"
fi
