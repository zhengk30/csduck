project('csduck', 'cpp', default_options: ['warning_level=2'])

## Source files
client_source_files = ['src/socket.cpp', 'src/client.cpp']
server_source_files = ['src/socket.cpp', 'src/server.cpp']

## Dependencies
duckdb_dep = declare_dependency(
    include_directories: include_directories('third-party/duckdb'),
    link_args: ['-L/users/zhengk30/csduck/third-party/duckdb', '-lduckdb']
)

executable('client', client_source_files)
executable('server', 
    dependencies: [duckdb_dep],
    sources: server_source_files)
