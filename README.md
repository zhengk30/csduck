# csduck

This project aims to evaluate DuckDB's query execution performance under the scenario where a client sends a SQL query over the network to a server that hosts DuckDB (*Currently, only selection queries with RegEx filters are evaluated. Stay tuned for supporting evaluation on a more generalized set of queries*). By default, the round-trip time is evaluated. The user can also specify whether timing should be enabled for each operator on the server side.

### Platform
- Ubuntu 20.04, ARM version

### Dependencies
- Nlohmann JSON parser
- DuckDB's C/C++ API

### Setup
- In `csduck/src/client.cpp`, modify the `SERVER_IPADDR` macro to the IP address of the machine that hosts DuckDB.
- In `csduck/src/server.cpp`, modify the `CONNECTION_FROM` macro to specify which client the user wants to exclusively connect with. Otherwise, the server will accept connections from any clients.
- In `meson.build`, change the path in `link_args` to the one where the `third-party/duckdb` directory lives on your machine (i.e., `-L/full/path/to/csduck/third-party/duckdb`).

### Build and Run
- Run `source config.sh` to get all the dependencies and path configurations set up. Note that running `./config.sh` won't work because the shell forks a child process where the changes to environment variables are only applied in the subshell and not to the shell itself.
- Open two terminal sessions and run the client and the host separately.
  - Build and run the server first by running `./run.sh server` so the client can find the connection. The server can take an optional command-line argument to specify whether the query execution should be profiled component-wise (i.e., whether each operator is timed apart from the query's end-to-end latency). If no argument is passed to the server process, the component-wise timing is disabled. Otherwise, pass in 1 to enable it.
  - On the client side, run `./run.sh client`.

### Notes
- If you wish to create input database files, create `csduck/inputs/` folder and dump the database files there.
- To configure required dependencies, refer to the guides under `docs/`.

