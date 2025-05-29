#include "third-party/duckdb/duckdb.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// db connection setup/teardown
void setup_db_connection(duckdb_database* db, duckdb_connection* conn, const char* path);
void teardown_db_connection(duckdb_database* db, duckdb_connection* conn);

// table generator
void generate_column(duckdb_connection conn, const char* sf_str, const char* dump);
void generate_db_file(duckdb_connection conn, const char* src);

int main() {
    duckdb_database db;
    duckdb_connection conn;
    duckdb_state state;
    duckdb_result result;

    // read and check the version (should be v1.2.x)
    setup_db_connection(&db, &conn, NULL);
    duckdb_query(conn, "PRAGMA version;", &result);
    char** version = (char **)duckdb_column_data(&result, 0);
    printf("version=%s\n", version[0]);
    teardown_db_connection(&db, &conn);
    
    // generate the target column
    setup_db_connection(&db, &conn, NULL);
    generate_column(conn, "5", "comment_sf5.csv");
    teardown_db_connection(&db, &conn);

    setup_db_connection(&db, &conn, "inputs/comment_sf5.db");
    generate_db_file(conn, "comment_sf5.csv");
    teardown_db_connection(&db, &conn);

    return 0;
}

void setup_db_connection(duckdb_database* db, duckdb_connection* conn, const char* path) {
    assert(duckdb_open(path, db) != DuckDBError);
    assert(duckdb_connect(*db, conn) != DuckDBError);
}

void teardown_db_connection(duckdb_database* db, duckdb_connection* conn) {
    duckdb_disconnect(conn);
    duckdb_close(db);
}

void generate_column(duckdb_connection conn, const char* sf_str, const char* dump) {
    char copy2query[1024] = {0};
    const char* copy = "COPY (SELECT l_comment FROM lineitem) TO '";
    const char* header = "' (header, delimiter ',');";
    memcpy(copy2query, copy, strlen(copy));
    memcpy(copy2query + strlen(copy), dump, strlen(dump));
    memcpy(copy2query + strlen(copy) + strlen(dump), header, strlen(header));

    char dbgen_query[64] = {0};
    const char* dbgen ="CALL dbgen(sf=";
    memcpy(dbgen_query, dbgen, strlen(dbgen)); 
    memcpy(dbgen_query + strlen(dbgen), sf_str, strlen(sf_str));
    memcpy(dbgen_query + strlen(dbgen) + strlen(sf_str), ");", 2);
    // printf("%s\n", dbgen_query);

    duckdb_query(conn, "INSTALL tpch; LOAD tpch;", NULL);
    duckdb_query(conn, dbgen_query, NULL);
    duckdb_query(conn, copy2query, NULL);
}

void generate_db_file(duckdb_connection conn, const char* src) {
    char create_query[1024] = {0};
    const char* create = "CREATE TABLE comment AS SELECT * FROM '";
    memcpy(create_query, create, strlen(create));
    memcpy(create_query + strlen(create), src, strlen(src));
    memcpy(create_query + strlen(create) + strlen(src), "';", 2);

    duckdb_query(conn, "PRAGMA force_compression='uncompressed';", NULL);
    duckdb_query(conn, create_query, NULL);
}
