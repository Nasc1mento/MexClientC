#ifndef MEX_CLIENT_H
#define MEX_CLIENT_H

#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/err.h"

#define THING_ID            CONFIG_THING_ID

#define BUFFER_SIZE             536
#define OPERATION_SIZE          13
#define LITERAL_STRING_SIZE     32

#define SIZE_STR            11

#define CONNECTED           0
#define NOT_CONNECTED       1

enum v_return {
    MEX_OK = 0,
            CONN_ERR,
            SEND_ERROR,
            RECV_ERROR,
            PARAM_ERROR
};

struct mex_client {
    uint8_t sock_fd;
    uint8_t st;
};

struct mex_client mex_connect(char*, unsigned short int);
uint8_t mex_publish(const struct mex_client *, const char*, const char*);
uint8_t mex_subscribe(const struct mex_client *, const char*);
#endif