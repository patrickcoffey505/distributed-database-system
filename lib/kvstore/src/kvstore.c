#include "kvstore.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void error_exit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

KeyValue** create_table() {
    KeyValue **table = malloc(sizeof(KeyValue*) * TABLE_SIZE);
    if (table == NULL) {
        error_exit("Failed to allocate memory for hash table");
    }
    for (int i = 0; i < TABLE_SIZE; i++) {
        // clear table
        table[i] = NULL;
    }
    return table;
}

void free_table(KeyValue **table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        KeyValue *pair = table[i];
        while (pair) {
            KeyValue *temp = pair;
            pair = pair->next;
            free(temp);
        }
    }
    free(table);
}

unsigned int hash(const char *key) {
    unsigned long hash = HASH_VALUE;
    int c;
    while ((c = *key++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash % TABLE_SIZE;
}

char* get_value(KeyValue **table, const char *key) {
    unsigned int index = hash(key);
    KeyValue *pair = table[index];
    while (pair) {
        if (strcmp(pair->key, key) == 0) {
            return pair->value;
        }
        pair = pair->next;
    }
    return NULL;
}

void add_entry(KeyValue **table, const char *key, const char *value, int log_fd) {
    unsigned int index = hash(key);
    KeyValue *new_pair = malloc(sizeof(KeyValue));
    if (new_pair == NULL) {
        error_exit("Failed to allocate memory for new KeyValue");
    }
    strcpy(new_pair->key, key);
    strcpy(new_pair->value, value);
    new_pair->next = table[index];
    table[index] = new_pair;
    
    if (log_fd != -1) {
        log_entry(log_fd, key, value);
        fsync_log(log_fd);
    }
}

void fsync_log(int log_fd) {
    if (fsync(log_fd) == -1) {
        error_exit("Failed to sync log file");
    }
}

void log_entry(int log_fd, const char *key, const char *value) {
    uint32_t key_len = strlen(key) + 1;
    uint32_t value_len = strlen(value) + 1;

    if (write(log_fd, &key_len, sizeof(key_len)) != sizeof(key_len) ||
        write(log_fd, key, key_len) != key_len ||
        write(log_fd, &value_len, sizeof(value_len)) != sizeof(value_len) ||
        write(log_fd, value, value_len) != value_len) {
        error_exit("Failed to write log entry");
    }
}

void load_log(KeyValue **table, int log_fd) {
    if (lseek(log_fd, 0, SEEK_SET) == (off_t)-1) {
        error_exit("Failed to seek to the start of the log file");
    }
    uint32_t key_len, value_len;
    char key[KEY_SIZE], value[VALUE_SIZE];

    while (read(log_fd, &key_len, sizeof(key_len)) == sizeof(key_len)) {
        if (read(log_fd, key, key_len) != key_len) {
            error_exit("Failed to read key from log file");
        }

        if (read(log_fd, &value_len, sizeof(value_len)) != sizeof(value_len)) {
            error_exit("Failed to read value length from log file");
        }

        if (read(log_fd, value, value_len) != value_len) {
            error_exit("Failed to read value from log file");
        }

        add_entry(table, key, value, -1);
    }
}