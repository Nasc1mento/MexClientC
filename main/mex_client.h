#ifndef MEX_CLIENT_H
#define MEX_CLIENT_H

#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/err.h"

#define THING_ID            CONFIG_THING_ID


#define MAX_BUF_LEN         536
#define OPERATION_LEN       13
#define TOPIC_LEN           50
#define MESSAGE_LEN         100


typedef char invocation[MAX_BUF_LEN];

typedef char operation[OPERATION_LEN];
typedef uint8_t id;
typedef char topic[TOPIC_LEN];
typedef char message[MESSAGE_LEN];

typedef enum {
    MEX_OK  = 0,
            IPCONV_ERR,
            INIT_ERR,
            CONN_ERR,
            SEND_ERR,
            INCOMPLETE_SEND_ERR,
            RECV_ERR,
            NULL_VALUE_ERR,
            BUFFER_OVERFLOW_ERR,
} mex_info;

struct mex_client {
    uint8_t sock_fd;
    mex_info status;
};

// request handler
struct mex_client mex_connect(char *, unsigned short int);
uint8_t mex_send(uint8_t, const char *, size_t);
uint8_t mex_recv(uint8_t, char *, size_t);

// proxy
uint8_t mex_publish(const struct mex_client *, topic, message);
uint8_t mex_subscribe(const struct mex_client *, topic);
uint8_t unsubscribe(const struct mex_client *, topic);
#endif