#include "socket.hpp"
#include "/users/zhengk30/csduck/third-party/duckdb/duckdb.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

#define CONNECTION_FROM "0.0.0.0"
#define DBFILE_PATH "/users/zhengk30/csduck/inputs/comment_sf5.db"

void process_request(int client_sock, bool prof_brkdown_enabled);

int main(int argc, char** argv) {
    bool prof_brkdown_enabled = false;
    if (argc > 2) {
        fprintf(stderr, "usage: build/server [prof_brkdown_enabled]\n");
        exit(1);
    } else if (argc == 2) {
        prof_brkdown_enabled = atoi(argv[1]);
    }
    int listen_fd = server_init(PORT, CONNECTION_FROM);
    std::cout << "[server] listening with fd " << listen_fd << " on port " << PORT << '\n'; 
    int client_sock = accept_connection(listen_fd);
    std::cout << "[server] accepted connection from client with fd " << client_sock << '\n';
    process_request(client_sock, prof_brkdown_enabled);
    tear_down_connection(listen_fd);
    return 0;
}

void process_request(int client_sock, bool prof_brkdown_enabled) {
    char query[BUF_SIZE];
    int bytes_read = read(client_sock, query, BUF_SIZE);
    if (bytes_read < 0) {
        perror("read");
        tear_down_connection(client_sock);
        exit(1);
    }
    std::cout << "[server] received " << bytes_read << " bytes from client with fd " << client_sock << '\n';;

    // connect to duckdb and run query
    duckdb_database db;
    duckdb_connection conn;
    duckdb_state state;
    duckdb_result result;
    if (duckdb_open(DBFILE_PATH, &db) == DuckDBError) {
        std::cerr << "DuckDBError: failed to open " << DBFILE_PATH << '\n';
        tear_down_connection(client_sock);
        exit(1);
    }
    if (duckdb_connect(db, &conn) == DuckDBError) {
        std::cerr << "DuckDBError: failed to establish connection\n";
        tear_down_connection(client_sock);
        exit(1);
    }
    
    if (prof_brkdown_enabled) {
        duckdb_query(conn, "PRAGMA enable_profiling='json';", NULL);
        duckdb_query(conn, "PRAGMA profiling_output='output.json';", NULL);
    }
    // execute query
    auto start = chrono::high_resolution_clock::now();
    state = duckdb_query(conn, query, &result);
    if (state == DuckDBError) {
        std::cerr << "DuckDBError: failed to execute query\n";
        close(client_sock);
        exit(1);
    }
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "[server] query execution duration: " << elapsed.count() << " sec\n";

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
    std::cout << "[server] replied with " << bytes_written << " bytes\n";

    if (prof_brkdown_enabled) {
        std::ifstream output("output.json");
        json data = json::parse(output);
        auto latency = data["latency"];
        auto cpu_time = data["cpu_time"];
        auto aggregate_cpu_time = data["children"][0]["operator_timing"];
        auto filter_cpu_time = data["children"][0]["children"][0]["operator_timing"];
        auto scan_cpu_time = data["children"][0]["children"][0]["children"][0]["operator_timing"];
        std::cout << "[server] execution latency (e2e): " << latency << " sec\n";
        std::cout << "[server] execution latency (cpu): " << cpu_time << " sec\n";
        std::cout << "[server] aggregate operator latency: " << aggregate_cpu_time << " sec\n";
        std::cout << "[server] filter operator latency: " << filter_cpu_time << " sec\n";
        std::cout << "[server] scan operator latency: " << scan_cpu_time << " sec\n";
    }
}
