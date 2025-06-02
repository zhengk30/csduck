#pragma once

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <chrono>

#define PORT 8000
#define IPADDR "127.0.0.1"
#define BUF_SIZE 1024

using namespace std;

// helpers
int set_up_connetion(int port, const char* ip, struct sockaddr_in* peer);
void tear_down_connection(int server_sock);

// client-specific functions
int client_init(int port, const char* ip);
void client_send_query(int server_sock, const char* query);
int client_fetch_result(int server_sock);
void tear_down_connection(int server_sock);

// server-specific functions
int server_init(int port, const char* ip);
int accept_connection(int listen_fd);
