#include "client_request_handler.h"

uint8_t mex_connect(char* mex_broker_ip, unsigned short int mex_broker_port) {
    char addr_str[128];
    struct sockaddr_in broker_info;
    uint8_t sock_fd;

    broker_info.sin_addr.s_addr = inet_addr(mex_broker_ip);
    broker_info.sin_family = AF_INET;

    broker_info.sin_port = htons(mex_broker_port);
    inet_ntoa_r(broker_info.sin_addr, addr_str, sizeof(addr_str) - 1);

    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock_fd < 0) {
        close(sock_fd);
    }

    if (connect(sock_fd, (struct sockaddr *)&broker_info, sizeof(broker_info)) != 0) {
        close(sock_fd);
        return -1;
    } 

    return sock_fd;
}

uint8_t mex_send(uint8_t sock_fd, const char* payload, size_t size) {
    if (!payload) {
        return PARAM_ERROR;
    }

    char size_str[11];
    snprintf(size_str, 11, "%d", size);

    while (strlen(size_str) < 10) {
        memmove(size_str + 1, size_str, strlen(size_str) + 1);
        size_str[0] = '0';
    }

    size_t buffer_size = 10 + size;

    char buffer[buffer_size];
    strcpy(buffer, size_str);
    strcat(buffer, payload);

    return send(sock_fd, buffer, buffer_size, 0) == buffer_size ? MEX_OK : SEND_ERROR;
}


uint8_t mex_recv(uint8_t sock_fd, char* buffer, size_t len) {

    ssize_t b = recv(sock_fd, buffer, len - 1, 0);

    if (b <= 0) {
        close(sock_fd);
        return SEND_ERROR;
    }

    buffer[b] = '\0';
    return MEX_OK;
}
