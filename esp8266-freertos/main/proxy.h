#ifndef PROXY_H
#define PROXY_H

#include "client_request_handler.h"

uint8_t publish(const struct mex_client *mc, const char* topic, const char* message);
uint8_t subscribe(const struct mex_client *mc, const char* topic);
uint8_t check_msg(const struct mex_client *mc, char* buffer);

#endif // PROXY_H