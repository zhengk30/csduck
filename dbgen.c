#include "third-party/duckdb/duckdb.h"
#include <stdio.h>

#define COPY_COLUMN_QUERY "COPY (SELECT l_comment FROM lineitem) TO 'comment_sf1.csv' (header, delimiter ',');"
#define CREATE_TABLE_QUERY "CREATE TABLE comment AS SELECT * FROM 'comment_sf1.csv';"

int main() {
    duckdb_database db;
    duckdb_connection conn;
    duckdb_state state;
    duckdb_result result;

    if (duckdb_open(NULL, &db) == DuckDBError) {
    
    }
    if (duckdb_connect(db, &conn) == DuckDBError) {
    
    }
    duckdb_query(conn, "INSTALL tpch; LOAD tpch;", NULL);
    duckdb_query(conn, "CALL dbgen(sf=1);", NULL);
    duckdb_query(conn, COPY_COLUMN_QUERY, NULL);
    duckdb_disconnect(&conn);
    duckdb_close(&db);

    if (duckdb_open("inputs/comment_sf1.db", &db) == DuckDBError) {
    
    }
    if (duckdb_connect(db, &conn) == DuckDBError) {
    
    }
    duckdb_query(conn, "PRAGMA force_compression='uncompressed';", NULL);
    duckdb_query(conn, CREATE_TABLE_QUERY, NULL);
    duckdb_disconnect(&conn);
    duckdb_close(&db);

    return 0;
}
