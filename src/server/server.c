#include "protocol.h"
#include "kvstore.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>

#define LOG_FILE "db.log"
#define REQUEST_QUEUE_SIZE 5
#define BUFFER_SIZE 256

static KeyValue** kv_store;

void start_server(const char* ip, int port) {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    
    // Initialization
    kv_store = create_table();
    
    int log_fd = open(LOG_FILE, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
    if (log_fd == -1) {
        error_exit("Failed to open log file");
    }
    load_log(kv_store, log_fd);
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip);
    address.sin_port = htons(port);
    
    // bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Start listening for connections
    if (listen(server_fd, REQUEST_QUEUE_SIZE) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on %s:%d\n", ip, port);

    while (1) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        printf("New client connected\n");

        while (1) {
            ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
            
            if (bytes_received > 0) {
                buffer[bytes_received] = '\0';
                handle_request(buffer, client_socket, kv_store, log_fd);
            } else if (bytes_received == 0) {
                break;
            } else {
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    break;
                }
            }
        }

        printf("Client disconnected\n");
        close(client_socket);
    }

    // Unreachable code
    free_table(kv_store);
    close(log_fd);
    close(server_fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    
    start_server(ip, port);

    return 0;
}