#include "socket.hpp"

#define QUERY "SELECT COUNT(l_comment) FROM comment WHERE l_comment like '%regular%';"
#define SERVER_IPADDR "128.110.216.201"

void client_send_query(int server_sock, const char* query);
int client_fetch_result(int server_sock);

int main() {
    std::cout << "[client] setting up server's socket...\n";
    int server_sock = client_init(PORT, SERVER_IPADDR);
    std::cout << "[client] server socket is " << server_sock << ", about to send a query to the server...\n";

    uint64_t checksum;
    assert(read(server_sock, &checksum, sizeof(uint64_t)) >= 0);
    assert(checksum == 0xdeadbeef);
    
    auto start = chrono::high_resolution_clock::now();
    client_send_query(server_sock, QUERY);
    uint64_t result = client_fetch_result(server_sock);
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double> rrt = end - start;
    std::cout << "[client] " << QUERY << " --> " << result << " (RRT " << rrt.count() << " sec)\n";

    tear_down_connection(server_sock);
    return 0;
}

void client_send_query(int server_sock, const char* query) {
    char buffer[BUF_SIZE] = {0};
    memcpy(buffer, query, strlen(query));
    if (write(server_sock, buffer, strlen(query)) < 0) {
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
