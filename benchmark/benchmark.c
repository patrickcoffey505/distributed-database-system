#include "client_lib.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>

#define VERBOSE 0
#define KEY_SIZE 64
#define READ_TIME_FILE "benchmark/read_times.json"
#define NUM_KVPAIRS 20000
const int benchmark_n[] = { 10, 100, 1000, 10000, 100000, 1000000 };
const int num_benchmarks = 6;

void remove_file_if_exists(const char *filename) {
    if (remove(filename) == -1 && errno != ENOENT) {
        fprintf(stderr, "Failed to remove file %s: %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void save_results_to_json(long *read_times) {
    FILE *file = fopen(READ_TIME_FILE, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    fprintf(file, "{\n");

    fprintf(file, "  \"benchmark_n\": [");
    for (int i = 0; i < num_benchmarks; ++i) {
        if (i > 0) {
            fprintf(file, ", ");
        }
        fprintf(file, "%d", benchmark_n[i]);
    }
    fprintf(file, "],\n");
    
    fprintf(file, "  \"read_times\": [");
    for (int i = 0; i < num_benchmarks; ++i) {
        if (i > 0) {
            fprintf(file, ", ");
        }
        fprintf(file, "\"%ld\"", read_times[i]);
    }
    fprintf(file, "]\n");

    fprintf(file, "}\n");
    
    fclose(file);
}

void run_benchmarks(int sock) {
    struct timeval start, end;
    int num_reads;
    int random_num;
    char request[256];
    long *read_times = malloc(num_benchmarks * sizeof(long));

    if (read_times == NULL) {
        perror("Failed to allocate memory for timing array");
        exit(EXIT_FAILURE);
    }

    remove_file_if_exists(READ_TIME_FILE);

    for (int i = 0; i < NUM_KVPAIRS; i++) {
        snprintf(request, sizeof(request), "PUT key%d value%d", i, i);
        send_request(sock, request);
        receive_response(sock, VERBOSE);
    }

    for (int i = 0; i < num_benchmarks; i++) {
        srand(time(0));

        num_reads = benchmark_n[i];
        printf("benchmarking for nreads = %d\n", num_reads);

        gettimeofday(&start, NULL);
        for (int n = 0; n < num_reads; n++) {
            random_num = (rand() % NUM_KVPAIRS) + 1;

            snprintf(request, sizeof(request), "GET key%d", random_num);
            send_request(sock, request);
            receive_response(sock, VERBOSE);
        }
        gettimeofday(&end, NULL);

        read_times[i] = ((end.tv_sec - start.tv_sec) * 1000000L + end.tv_usec) - start.tv_usec;
        printf("results for nreads = %d: total_time = %ld microseconds\n\n", num_reads, read_times[i]);
    }

    save_results_to_json(read_times);
    free(read_times);
}

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

    printf("Connected to server at %s:%d\n\n", server_ip, server_port);

    run_benchmarks(sock);

    close(sock);
    return 0;
}