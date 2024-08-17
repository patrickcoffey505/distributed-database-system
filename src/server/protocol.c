#include "protocol.h"
#include "kvstore.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_SIZE 256

void handle_get(const char *key, int client_socket, KeyValue **kv_store) {
    char *value = get_value(kv_store, key);
    char response[BUFFER_SIZE];

    if (value) {
        snprintf(response, BUFFER_SIZE, "OK %s\n", value);
    } else {
        snprintf(response, BUFFER_SIZE, "NOT_FOUND\n");
    }

    send(client_socket, response, strlen(response), 0);
}

void handle_put(const char *key, const char *value, int client_socket, KeyValue **kv_store, int log_fd) {
    add_entry(kv_store, key, value, log_fd);
    const char *response = "STORED\n";
    send(client_socket, response, strlen(response), 0);
}

void handle_contains(const char *key, int client_socket, KeyValue **kv_store) {
    char response[BUFFER_SIZE];

    if (get_value(kv_store, key) != NULL) {
        snprintf(response, BUFFER_SIZE, "EXISTS\n");
    } else {
        snprintf(response, BUFFER_SIZE, "NOT_FOUND\n");
    }

    send(client_socket, response, strlen(response), 0);
}

void handle_shutdown(int client_socket, KeyValue **kvstore, int log_fd, int server_fd) {
    send(client_socket, "Server is shutting down...\n", strlen("Server is shutting down...\n"), 0);
    close(client_socket);
    printf("Client disconnected\n");
    printf("Server is shutting down...\n");
    free_table(kvstore);
    close(log_fd);
    close(server_fd);
    exit(EXIT_SUCCESS);
}

void handle_request(const char *request, int client_socket, KeyValue **kv_store, int log_fd, int server_fd) {
    char command[BUFFER_SIZE];
    char key[KEY_SIZE];
    char value[VALUE_SIZE];

    if (sscanf(request, "%s %63s %63s", command, key, value) < 1) {
        const char *error_response = "ERROR Invalid command\n";
        send(client_socket, error_response, strlen(error_response), 0);
        return;
    }

    if (strcmp(command, "GET") == 0) {
        handle_get(key, client_socket, kv_store);
    } else if (strcmp(command, "PUT") == 0) {
        handle_put(key, value, client_socket, kv_store, log_fd);
    } else if (strcmp(command, "CONTAINS") == 0) {
        handle_contains(key, client_socket, kv_store);
    } else if (strcmp(command, "SHUTDOWN") == 0) {
        handle_shutdown(client_socket, kv_store, log_fd, server_fd);
    } else {
        const char *error_response = "ERROR Unknown command\n";
        send(client_socket, error_response, strlen(error_response), 0);
    }
}
