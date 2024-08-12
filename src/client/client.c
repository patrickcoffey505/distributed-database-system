#include "client_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define VERBOSE 1

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sock;
    struct sockaddr_in server_address;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error_exit("Socket creation error");
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0) {
        error_exit("Invalid address/ Address not supported");
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        error_exit("Connection failed");
    }

    printf("Connected to server at %s:%d\n", server_ip, server_port);

    char request[BUFFER_SIZE];
    while (1) {

        printf("Enter command (GET, PUT, CONTAINS) followed by key and value (if applicable): ");
        if (fgets(request, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        request[strcspn(request, "\n")] = 0;

        send_request(sock, request);
        receive_response(sock, VERBOSE);
    }

    close(sock);
    return 0;
}
