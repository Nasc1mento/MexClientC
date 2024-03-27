#include "mex_client.h"



struct mex_client mex_connect(char* mex_broker_ip, unsigned short int mex_broker_port) {
    char addr_str[128];
    struct mex_client mc;
    struct sockaddr_in broker_info;

    broker_info.sin_addr.s_addr = inet_addr(mex_broker_ip);
    broker_info.sin_family = AF_INET;
    broker_info.sin_port = htons(mex_broker_port);
    inet_ntoa_r(broker_info.sin_addr, addr_str, sizeof(addr_str) - 1);

    mc.sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (mc.sock_fd < 0) {
        mc.status = INIT_ERR;
    }

    if (connect(mc.sock_fd, (struct sockaddr *)&broker_info, sizeof(broker_info)) != 0) {
        close(mc.sock_fd);
        mc.status = CONN_ERR;
    } else {
        mc.status = MEX_OK;
    }
    return mc;
}

uint8_t mex_send(uint8_t sock_fd, const char* payload, size_t len) {
    if (!payload) {
        return NULL_VALUE_ERR;
    }

    ssize_t b = send(sock_fd, payload, len, 0);

    if (b < 0) {
        return SEND_ERR;
    }
    
    if ((size_t) b < len) {
        return INCOMPLETE_SEND_ERR;
    }
    return MEX_OK;
}


uint8_t mex_recv(uint8_t sock_fd, char* buffer, size_t len) {
    ssize_t b = recv(sock_fd, buffer, len - 1, 0);

    if (b < 0) {
        return RECV_ERR;
    }
    
    if (b == 0) {
        return RECV_ERR;
    }

    buffer[b] = '\0';
    return MEX_OK;
}


uint8_t mex_publish(const struct mex_client *mc, topic t, message m) {
    operation o = "Publish";
    invocation i;
    snprintf(i, MAX_BUF_LEN,
    "{\"OP\":\"%s\",\"THING_ID\":\"%d\",\"TOPICS\":\"%s\",\"MSG\":\"%s\"}",
    o, THING_ID, t, m);
    
    return mex_send(mc->sock_fd, i, MAX_BUF_LEN);
}

uint8_t mex_subscribe(const struct mex_client *mc, topic t) {
    operation o = "Subscribe";
    invocation i;
    snprintf(i, MAX_BUF_LEN,
    "{\"OP\":\"%s\",\"THING_ID\":\"%d\",\"TOPICS\":\"%s\",\"MSG\":\"%s\"}",
    o, THING_ID, t, "");
    return mex_send(mc->sock_fd, i, MAX_BUF_LEN);
}