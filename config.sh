#!/bin/bash

so_path=/users/zhengk30/csduck/third-party/duckdb/

## Unzip DuckDB's C/C++ API
cd third-party/duckdb
if ! [[ -f duckdb.h && -f duckdb.hpp && -f libduckdb.so ]]; then
    unzip libduckdb-linux-aarch64.zip
fi
cd ../..

## Set up LD_LIBRARY_PATH if needed
if ! [[ $LD_LIBRARY_PATH == *"$so_path"* ]]; then
    LD_LIBRARY_PATH=/usr/local/lib
    export LD_LIBRARY_PATH=$so_path:$LD_LIBRARY_PATH
fi
# echo "$LD_LIBRARY_PATH"

## TODO: Create `inputs` directory 

