# csduck

## Platform
- Ubuntu 20.04, ARM version

## Dependencies
- Nlohmann JSON parser
- DuckDB's C/C++ API

## Notes
- If you wish to create input database files, create `csduck/inputs/` folder and dump the database files there.
- To configure required dependencies, refer to the guides under `docs/`.
- For configuring `LD_LIBRARY_PATH`, running `./config.sh` wouldn't work because the shell forks a child process where the changes are applied in the child. Run `source config.sh` to persist the changes to the shell.

