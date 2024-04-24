#ifndef CLIENT_REQUEST_HANDLER_H
#define CLIENT_REQUEST_HANDLER_H

#include <lwip/sys.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/err.h>

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

struct mex_client mex_connect(char* broker_ip, unsigned short int broker_port);
uint8_t mex_send(uint8_t sock_fd, const char* payload, size_t size);
uint8_t mex_recv(uint8_t sock_fd, char* buffer, size_t len);


#endif