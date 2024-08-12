#ifndef KVSTORE_H
#define KVSTORE_H

#include <stdint.h>
#include <sys/types.h>

#define KEY_SIZE 64
#define VALUE_SIZE 64
#define TABLE_SIZE 10000
#define HASH_VALUE 5381

typedef struct KeyValue {
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    struct KeyValue *next;
} KeyValue;

unsigned int hash(const char *key);
void log_entry(int log_fd, const char *key, const char *value);
void fsync_log(int log_fd);
void error_exit(const char *message);

// public
KeyValue** create_table();
void free_table(KeyValue **table);
void add_entry(KeyValue **table, const char *key, const char *value, int log_fd);
char* get_value(KeyValue **table, const char *key);
void load_log(KeyValue **table, int log_fd);

#endif