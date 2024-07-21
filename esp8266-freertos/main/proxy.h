#ifndef PROXY_H
#define PROXY_H

#include "client_request_handler.h"

struct mex_client {
    uint8_t sock_fd;
    uint8_t st;
};


struct mex_client create_connection(char* broker_ip, unsigned short int broker_port);
uint8_t publish(const struct mex_client *mc, const char* topic, const char* message);
uint8_t subscribe(const struct mex_client *mc, const char* topic);
uint8_t check_msg(const struct mex_client *mc, const char *topics, char* buffer, const size_t len);

#endif // PROXY_H