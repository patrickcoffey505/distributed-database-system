#ifndef CLIENT_LIB_H
#define CLIENT_LIB_H

void send_request(int sock, const char *request);
void receive_response(int sock, int verbose);
void error_exit(const char *message);

#endif