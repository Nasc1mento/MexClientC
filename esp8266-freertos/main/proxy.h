#ifndef PROXY_H
#define PROXY_H

#include "client_request_handler.h"

#define BUFFER_SIZE             536
#define OPERATION_SIZE          13
#define LITERAL_STRING_SIZE     32
#define SIZE_STR                11

#define THING_ID            CONFIG_THING_ID

uint8_t publish(const struct mex_client *mc, const char* topic, const char* message);
uint8_t subscribe(const struct mex_client *mc, const char* topic);
uint8_t check_msg(const struct mex_client *mc, char* buffer);

#endif // PROXY_H