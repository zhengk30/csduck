#!/bin/bash

common_path=/users/zhengk30/csduck/
duckdb_path=/users/zhengk30/.duckdb/
duckdb_cli_path=/users/zhengk30/.duckdb/cli/latest

so_path=${common_path}third-party/duckdb/
inputs_path=${common_path}inputs/
db_file_path=${common_path}inputs/comment_sf1.db
duckdb_api_path=${comment_path}third-party/duckdb

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

## Install DuckDB's CLI
if ! [ -d "$duckdb_path" ]; then
    curl https://install.duckdb.org | sh
fi

if ! [[ $PATH == *"$duckdb_cli_path"* ]]; then
    export PATH=$duckdb_cli_path:$PATH
fi

## Create `inputs` directory if none exists
if ! [ -d "$inputs_path" ]; then
    mkdir $inputs_path
fi

## Create a database file if none exists
if ! [ -e "$db_file_path" ]; then
    gcc dbgen.c -o dbgen -L${duckdb_api_path} -lduckdb
    ./dbgen
    rm -f comment_sf1.csv
    rm -f dbgen
fi
