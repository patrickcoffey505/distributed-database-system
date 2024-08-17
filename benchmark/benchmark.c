#include "client_lib.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <xxhash.h>

#define VERBOSE 0
#define KEY_SIZE 64
#define STATS_FILE "benchmark/stats.json"
#define NUM_READS 100000
#define NUM_WRITES 100000

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

void save_results_to_json(int num_servers, int replication_factor, long read_time, long write_time) {
    FILE *file = fopen(STATS_FILE, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "{\n");
    fprintf(file, "  \"num_servers\": %d,\n", num_servers);
    fprintf(file, "  \"replication_factor\": %d,\n", replication_factor);
    fprintf(file, "  \"read_time\": %ld,\n", read_time);
    fprintf(file, "  \"write_time\": %ld\n", write_time);
    fprintf(file, "},\n");
    
    fclose(file);
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

    if (replication_factor > num_connected_servers) {
        printf("Not enough servers. Shutting down.\n");
        return 0;
    }

    struct timeval start, end;
    int random_num;
    char request[256];
    long read_time;
    long write_time;

    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_WRITES; i++) {
        char key[KEY_SIZE];
        snprintf(key, KEY_SIZE, "key%d", i);

        int selected_servers[num_connected_servers];
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

        snprintf(request, sizeof(request), "PUT key%d value%d", i, i);

        for (int i = 0; i < replication_factor; i++) {
            send_request(selected_servers[i], request);
            receive_response(selected_servers[i], 0);
        }
    }
    gettimeofday(&end, NULL);

    write_time = ((end.tv_sec - start.tv_sec) * 1000000L + end.tv_usec) - start.tv_usec;
    printf("results for %d writes with num_servers = %d and repl factor %d: total_time = %ld microseconds\n\n", NUM_WRITES, num_connected_servers, replication_factor, write_time);

    srand(time(0));
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_READS; i++) {
        char key[KEY_SIZE];
        int selected_servers[num_connected_servers];
        uint32_t hashes[num_connected_servers];
        int indexes[num_connected_servers];

        random_num = (rand() % NUM_WRITES) + 1;
        snprintf(key, KEY_SIZE, "key%d", random_num);

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

        snprintf(request, sizeof(request), "GET key%d", random_num);
        int selected_index = rand() % replication_factor;
        send_request(selected_servers[selected_index], request);
        receive_response(selected_servers[selected_index], VERBOSE);
    }
    gettimeofday(&end, NULL);

    read_time = ((end.tv_sec - start.tv_sec) * 1000000L + end.tv_usec) - start.tv_usec;
    printf("results for %d reads with num_servers = %d and repl factor %d: total_time = %ld microseconds\n\n", NUM_READS, num_connected_servers, replication_factor, read_time);

    save_results_to_json(num_servers, replication_factor, read_time, write_time);

    for (int i = 0; i < num_servers; i++) {
        send_request(servers[i].sock, "SHUTDOWN");
        receive_response(servers[i].sock, VERBOSE);
        close(servers[i].sock);
    }

    return 0;
}