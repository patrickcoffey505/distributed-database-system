#include "client_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256

void error_exit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void send_request(int sock, const char *request) {
    if (send(sock, request, strlen(request), 0) < 0) {
        error_exit("send failed");
    }
}

void receive_response(int sock, int verbose) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);

    if (bytes_received < 0) {
        error_exit("recv failed");
    } else if (bytes_received == 0) {
        printf("Server closed the connection\n");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    if (verbose) {
        printf("Response: %s", buffer);
    }
}
