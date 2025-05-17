#include "socket.h"
#include <time.h>

#define QUERY "SELECT COUNT(l_comment) FROM comment WHERE l_comment like '%regular%';"
#define SERVER_IPADDR "128.110.216.206"

void client_send_query(int server_sock, const char* query);
int client_fetch_result(int server_sock);

int main() {
    printf("[client] setting up server's socket...\n");
    int server_sock = client_init(PORT, SERVER_IPADDR);
    printf("[client] server socket is %d, about to send a query to the server...\n", server_sock);

    clock_t begin = clock();
    client_send_query(server_sock, QUERY);
    uint64_t result = client_fetch_result(server_sock);
    clock_t end = clock();
    double duration = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("[client] %s --> %llu (%f sec)\n", QUERY, result, duration);
    // tear_down_connection(server_sock);
    return 0;
}

void client_send_query(int server_sock, const char* query) {
    char buffer[BUF_SIZE] = {0};
    memcpy(buffer, query, strlen(query));
    if (send(server_sock, buffer, strlen(query), 0) < 0) {
        perror("send");
        close(server_sock);
        exit(1);
    }
}

int client_fetch_result(int server_sock) {
    int result;
    if (read(server_sock, &result, sizeof(int)) < 0) {
        perror("read");
        close(server_sock);
        exit(1);
    }
    return result;
}
