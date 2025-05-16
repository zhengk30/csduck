#include "socket.h"
#include "third-party/duckdb/osx/duckdb.h"

#define CONNECTION_FROM "0.0.0.0"
#define DBFILE_PATH "/users/zhengk30/csduck/inputs/tpch_lineitem_comment_sf1.db"

void process_request(int client_sock);

int main() {
    int listen_fd = server_init(CONNECTION_FROM, IPADDR);
    printf("[server] listening on %d...\n", listen_fd);
    int client_sock = accept_connection(listen_fd);
    printf("[server] accepted connection from client %d\n", client_sock);
    process_request(client_sock);
    tear_down_connection(listen_fd);
    return 0;
}

void process_request(int client_sock) {
    char query[BUF_SIZE];
    int bytes_read = read(client_sock, query, BUF_SIZE);
    if (bytes_read < 0) {
        perror("read");
        tear_down_connection(client_sock);
        exit(1);
    }
    printf("[server] received %d bytes from client %d\n", bytes_read, client_sock);

    // connect to duckdb and run query
    duckdb_database db;
    duckdb_connection conn;
    duckdb_state state;
    duckdb_result result;
    if (duckdb_open(DBFILE_PATH, &db) == DuckDBError) {
        fprintf(stderr, "DuckDBError: failed to open %s\n", DBFILE_PATH);
        tear_down_connection(client_sock);
        exit(1);
    }
    if (duckdb_connect(db, &conn) == DuckDBError) {
        fprintf(stderr, "DuckDBError: failed to establish connection\n");
        tear_down_connection(client_sock);
        exit(1);
    }

    // execute query
    state = duckdb_query(conn, query, &result);
    if (state == DuckDBError) {
        fprintf(stderr, "DuckDBError: failed to execute query\n");
        tear_down_connection(client_sock);
        exit(1);
    }

    // retrive result from row 0, column 0 of the returned table
    uint64_t count = 0;
    if (duckdb_row_count(&result) > 0) {
        count = duckdb_value_uint64(&result, 0, 0);
    }

    // reply to client with count
    int bytes_written = write(client_sock, &count, sizeof(uint64_t));
    if (bytes_written < 0) {
        perror("write");
        tear_down_connection(client_sock);
        exit(1);
    }
    duckdb_destroy_result(&result);
    printf("[server] replied to client %d with %d bytes\n", client_sock, bytes_written);
}
