# Configuration
- Go to `csduck/third-party/duckdb`. You will notice the `libduckdb-linux-aarch64.zip` file in that folder.
- Unzip it. There will be two header files (`duckdb.h` and `duckdb.hpp`), a static library (`libduckdb_static.a`), and a dynamically linked library (`libduckdb.so`).
- `csduck` uses DuckDB's C API, so `duckdb.h` is included.
- If you are encountering `error while loading shared libraries: libduckdb.so: cannot open shared object file: No such file or directory`, refer to [this post](https://stackoverflow.com/questions/480764/linux-error-while-loading-shared-libraries-cannot-open-shared-object-file-no-s).
  
