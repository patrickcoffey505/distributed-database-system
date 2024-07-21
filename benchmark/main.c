#include "kvstore.h"
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <errno.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

#define LOG_FILE "db.log"
#define NUM_READS 1000000
#define NUM_WRITES 1000000
#define BENCHMARK_DATA_DIR "_benchmark_data"
#define WRITE_TIME_FILE BENCHMARK_DATA_DIR "/write_times.txt"
#define READ_TIME_FILE BENCHMARK_DATA_DIR "/read_times.txt"

void remove_file_if_exists(const char *filename) {
    if (remove(filename) == -1 && errno != ENOENT) {
        fprintf(stderr, "Failed to remove file %s: %s\n", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void calculate_and_print_stats(long *times, size_t count, const char *label) {
    double sum = 0.0;
    double sum_sq = 0.0;
    for (size_t i = 0; i < count; i++) {
        sum += times[i];
        sum_sq += times[i] * times[i];
    }

    double mean = sum / count;
    double variance = (sum_sq / count) - (mean * mean);
    double stddev = sqrt(variance);

    printf("%s - Average time: %f microseconds\n", label, mean);
    printf("%s - Standard deviation: %f microseconds\n", label, stddev);
}

void write_times_to_file(const char *filename, long *times, size_t count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < count; i++) {
        fprintf(file, "%ld\n", times[i]);
    }

    fclose(file);
}

void benchmark() {
    struct timeval start, end;
    
    long *write_times = malloc(NUM_WRITES * sizeof(long));
    long *read_times = malloc(NUM_READS * sizeof(long));

    if (write_times == NULL || read_times == NULL) {
        perror("Failed to allocate memory for timing arrays");
        exit(EXIT_FAILURE);
    }

    // Remove existing log file if it exists
    remove_file_if_exists(LOG_FILE);

    struct stat st = {0};

    if (stat(BENCHMARK_DATA_DIR, &st) == -1) {
        if (mkdir(BENCHMARK_DATA_DIR, 0700) == -1) {
            fprintf(stderr, "Failed to create directory %s: %s\n", BENCHMARK_DATA_DIR, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    // Remove existing benchmark data files if they exist
    remove_file_if_exists(WRITE_TIME_FILE);
    remove_file_if_exists(READ_TIME_FILE);

    // Create table and log file
    KeyValue **table = create_table();
    int log_fd = open(LOG_FILE, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
    if (log_fd == -1) {
        free_table(table);
        error_exit("Failed to open log file");
    }

    // Benchmark write operations
    for (int i = 0; i < NUM_WRITES; i++) {
        char key[KEY_SIZE], value[VALUE_SIZE];
        sprintf(key, "key%d", i);
        sprintf(value, "value%d", i);
        
        struct timeval write_start, write_end;
        gettimeofday(&write_start, NULL);
        add_entry(table, key, value, log_fd);
        gettimeofday(&write_end, NULL);
        
        write_times[i] = ((write_end.tv_sec - write_start.tv_sec) * 1000000L + write_end.tv_usec) - write_start.tv_usec;
    }

    printf("Write operations:\n");
    calculate_and_print_stats(write_times, NUM_WRITES, "Write");

    // Benchmark read operations
    for (int i = 0; i < NUM_READS; i++) {
        char key[KEY_SIZE];
        sprintf(key, "key%d", i);
        
        struct timeval read_start, read_end;
        gettimeofday(&read_start, NULL);
        get_value(table, key);
        gettimeofday(&read_end, NULL);
        
        read_times[i] = ((read_end.tv_sec - read_start.tv_sec) * 1000000L + read_end.tv_usec) - read_start.tv_usec;
    }

    printf("Read operations:\n");
    calculate_and_print_stats(read_times, NUM_READS, "Read");

    // Load log file
    free_table(table);
    table = create_table();

    gettimeofday(&start, NULL);
    load_log(table, log_fd);
    gettimeofday(&end, NULL);
    printf("Initial load time (caching): %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + end.tv_usec) - start.tv_usec);

    // Load log file without caching
    close(log_fd);
    free_table(table);
    table = create_table();
    log_fd = open(LOG_FILE, O_RDONLY | O_SYNC);
    fcntl(log_fd, F_NOCACHE, 1);

    gettimeofday(&start, NULL);
    load_log(table, log_fd);
    gettimeofday(&end, NULL);
    printf("Cached load time (no caching): %ld microseconds\n", ((end.tv_sec - start.tv_sec) * 1000000L + end.tv_usec) - start.tv_usec);

    // log read and write times for python analysis
    write_times_to_file(WRITE_TIME_FILE, write_times, NUM_WRITES);
    write_times_to_file(READ_TIME_FILE, read_times, NUM_READS);

    // Clean up
    close(log_fd);
    remove_file_if_exists(LOG_FILE);
    free_table(table);
}

int main() {
    benchmark();
    return 0;
}