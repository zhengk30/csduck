#include "socket.h"

#define QUERY "SELECT COUNT(l_comment) FROM comment WHERE l_comment like '%regular%';"
#define SERVER_IPADDR "128.110.216.209"

int main() {
    printf("[client] setting up server's socket...\n");
    int server_sock = client_init(PORT, SERVER_IPADDR);
    printf("[client] server socket is %d, about to send a query to the server...\n", server_sock);
    client_send_query(server_sock, QUERY);
    uint64_t result = client_fetch_result(server_sock);
    printf("[client] %s --> %llu\n", QUERY, result);
    tear_down_connection(server_sock);
    return 0;
}