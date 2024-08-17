#include "client_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <xxhash.h>

#define BUFFER_SIZE 256
#define KEY_SIZE 64
#define VALUE_SIZE 64
#define VERBOSE 1
#define MAX_SERVERS 20

uint32_t hash_key(const char *key, const char *server_ip, int server_port) {
    char key_server_combined[BUFFER_SIZE];
    snprintf(key_server_combined, BUFFER_SIZE, "%s%s%d", key, server_ip, server_port);
    uint32_t hash = XXH32(key_server_combined, strlen(key_server_combined), 0);
    return hash;
}

int compare(void *context, const void *a, const void *b) {
    int idx1 = *(int*)a;
    int idx2 = *(int*)b;
    int *hash_array = (int *)context;
    return hash_array[idx2] - hash_array[idx1];
}

int main(int argc, char *argv[]) {
    int num_connected_servers = 0;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <replication_factor> <server_ip1> <server_port1> [<server_ip2> <server_port2> ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_servers = (argc - 1) / 2;
    int replication_factor = atoi(argv[1]);

    if (num_servers < replication_factor) {
        fprintf(stderr, "Error: Replication factor is greater than the number of servers provided.\n");
        exit(EXIT_FAILURE);
    }
    
    Server servers[num_servers];
    const char *server_ip;
    int server_port;

    for (int i = 0; i < num_servers; i++) {
        server_ip = argv[2 + i * 2];
        server_port = atoi(argv[3 + i * 2]);

        servers[i].sock = socket(AF_INET, SOCK_STREAM, 0);
        if (servers[i].sock < 0) {
            error_exit("Socket creation error");
        }

        servers[i].address.sin_family = AF_INET;
        servers[i].address.sin_port = htons(server_port);

        if (inet_pton(AF_INET, server_ip, &servers[i].address.sin_addr) <= 0) {
            error_exit("Invalid address/ Address not supported");
        }

        if (connect(servers[i].sock, (struct sockaddr *)&servers[i].address, sizeof(servers[i].address)) < 0) {
            error_exit("Connection failed");
        }

        strncpy(servers[i].ip, server_ip, INET_ADDRSTRLEN);
        servers[i].port = server_port;
        servers[i].connected = 1;

        printf("Connected to server at %s:%d\n", servers[i].ip, servers[i].port);
        num_connected_servers++;
    }

    char request[BUFFER_SIZE];
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    char command[BUFFER_SIZE];

    while (1) {

        printf("Enter command (GET, PUT, CONTAINS, SHUTDOWN) followed by key and value (if applicable): ");
        if (fgets(request, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        request[strcspn(request, "\n")] = 0;

        int selected_servers[num_connected_servers];
        if (sscanf(request, "%s %63s %63s", command, key, value) > 1) {
            if (replication_factor > num_connected_servers) {
                printf("Not enough servers. Shutting down.\n");
                break;
            }

            uint32_t hashes[num_connected_servers];
            int indexes[num_connected_servers];
            int j = 0;
            for (int i = 0; i < num_servers; i++) {
                if (servers[i].connected == 1) {
                    hashes[i] = hash_key(key, servers[i].ip, servers[i].port);
                    indexes[i] = j;
                    j++;
                }
            }
            qsort_r(indexes, num_connected_servers, sizeof(int), hashes, compare);
            
            for (int i = 0; i < replication_factor; i++) {
                int idx = indexes[i];
                selected_servers[i] = servers[idx].sock;
            }

            if (strcmp(command, "GET") == 0 || strcmp(command, "CONTAINS") == 0) {
                int selected_index = rand() % replication_factor;
                send_request(selected_servers[selected_index], request);
                receive_response(selected_servers[selected_index], VERBOSE);
            } else if (strcmp(command, "PUT") == 0) {
                for (int i = 0; i < replication_factor; i++) {
                    send_request(selected_servers[i], request);
                    receive_response(selected_servers[i], 0);
                }
            } else {
                printf("Unknown command.\n");
            }
        } else if (strcmp(command, "SHUTDOWN") == 0) {
            for (int i = 0; i < num_servers; i++) {
                send_request(servers[i].sock, "SHUTDOWN");
                receive_response(servers[i].sock, 1);
            }
            break;
        } else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}
