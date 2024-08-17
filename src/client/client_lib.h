#ifndef CLIENT_LIB_H
#define CLIENT_LIB_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>

#define BUFFER_SIZE 256

typedef struct {
    int sock;
    struct sockaddr_in address;
    char ip[INET_ADDRSTRLEN];
    int port;
    bool connected;
} Server;

void send_request(int sock, const char *request);
void receive_response(int sock, int verbose);
void error_exit(const char *message);

#endif