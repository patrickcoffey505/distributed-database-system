#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "kvstore.h"

void handle_request(const char *request, int client_socket, KeyValue **kv_store, int log_fd, int server_fd);

#endif
