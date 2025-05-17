project('csduck', 'c', default_options: ['warning_level=2'])

## Source files
client_source_files = ['src/socket.c', 'src/client.c']
server_source_files = ['src/socket.c', 'src/server.c']

## Dependencies
duckdb_dep = declare_dependency(
    include_directories: include_directories('third-party/duckdb'),
    link_args: ['-L/users/zhengk30/csduck/third-party/duckdb', '-lduckdb']
)

executable('client', client_source_files)
executable('server', 
    dependencies: [duckdb_dep],
    sources: server_source_files)
