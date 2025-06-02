#include "socket.hpp"
#include "/users/zhengk30/csduck/third-party/duckdb/duckdb.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

#define CONNECTION_FROM "0.0.0.0"
#define DBFILE_PATH "/proj/fardatalab-PG0/dbfiles/sf100.db"
#define IMDB_PATH "/proj/fardatalab-PG0/exported-data/sf50"

void process_request(int client_sock, bool prof_brkdown_enabled);
void process_request(duckdb_connection conn, int client_sock, bool prof_brkdown_enabled);
void setup_db_connection(duckdb_database* db, duckdb_connection* conn, const char* dbfile_path, const char* db_dir);
void teardown_db_connection(duckdb_database* db, duckdb_connection* conn);
void parse_stats(const char* src);

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

    uint64_t checksum = 0xdeadbeef;
    duckdb_database db;
    duckdb_connection conn;
    setup_db_connection(&db, &conn, NULL, IMDB_PATH);
    assert(write(client_sock, &checksum, sizeof(checksum)) >= 0);
    
    process_request(conn, client_sock, prof_brkdown_enabled);

    if (prof_brkdown_enabled) {
        parse_stats("output.json");
    }
    teardown_db_connection(&db, &conn);
    close(listen_fd);
    
    return 0;
}

void process_request(duckdb_connection conn, int client_sock, bool prof_brkdown_enabled) {
    char query[BUF_SIZE];
    int bytes_read = read(client_sock, query, BUF_SIZE);
    assert(bytes_read >= 0);
    std::cout << "[server] received " << bytes_read << " bytes from client with fd " << client_sock << '\n';;

    // connect to duckdb and run query
    duckdb_result result;
    if (prof_brkdown_enabled) {
        duckdb_query(conn, "PRAGMA enable_profiling='json';", NULL);
        duckdb_query(conn, "PRAGMA profiling_output='output.json';", NULL);
    }
    // execute query
    auto start = chrono::high_resolution_clock::now();
    assert(duckdb_query(conn, query, &result) != DuckDBError);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    std::cout << "[server] query execution duration: " << elapsed.count() << " sec\n";

    // retrive result from row 0, column 0 of the returned table
    uint64_t count = 0;
    if (duckdb_row_count(&result) > 0) {
        count = duckdb_value_uint64(&result, 0, 0);
    }
    
    // reply to client with count
    int bytes_written = write(client_sock, &count, sizeof(uint64_t));
    assert(bytes_written >= 0);
    duckdb_destroy_result(&result);
    std::cout << "[server] replied with " << bytes_written << " bytes\n";
}

void setup_db_connection(duckdb_database* db, duckdb_connection* conn, const char* dbfile_path, const char* db_dir) {
    assert(!dbfile_path || db_dir);  // if duckdb is to be run in memory, then db_dir should be specified
    assert(duckdb_open(dbfile_path, db) != DuckDBError);
    assert(duckdb_connect(*db, conn) != DuckDBError);
    if (!dbfile_path) {
        // import database
        char import_query[1024] = {0};
        char import[32] = "IMPORT DATABASE '";
        memcpy(import_query, import, strlen(import));
        memcpy(import_query + strlen(import), db_dir, strlen(db_dir));
        memcpy(import_query + strlen(import) + strlen(db_dir), "';", 2);
        assert(duckdb_query(*conn, import_query, NULL) != DuckDBError);
    }
}

void teardown_db_connection(duckdb_database* db, duckdb_connection* conn) {
    duckdb_disconnect(conn);
    duckdb_close(db);
}

void parse_stats(const char* src) {
    std::ifstream output(src);
    assert(output);
    json data = json::parse(output);
    auto latency = data["latency"];
    auto cpu_time = data["cpu_time"];
    auto aggregate_cpu_time = data["children"][0]["operator_timing"];
    auto filter_cpu_time = data["children"][0]["children"][0]["operator_timing"];
    auto scan_cpu_time = data["children"][0]["children"][0]["children"][0]["operator_timing"];
    std::cout << "======= Timing breakdown =======\n";
    std::cout << "[server] execution latency (e2e): " << latency << " sec\n";
    std::cout << "[server] execution latency (cpu): " << cpu_time << " sec\n";
    std::cout << "[server] aggregate operator latency: " << aggregate_cpu_time << " sec\n";
    std::cout << "[server] filter operator latency: " << filter_cpu_time << " sec\n";
    std::cout << "[server] scan operator latency: " << scan_cpu_time << " sec\n";
}
