#include "proxy.h"
#include "marshaller.h"



struct mex_client create_connection(char *broker_ip, unsigned short int broker_port) {
    struct mex_client mc;
    mc.sock_fd = mex_connect(broker_ip, broker_port);
    if (mc.sock_fd == -1) {
        mc.st = NOT_CONNECTED;
    } else {
        mc.st = CONNECTED;
    }
    return mc;
}

uint8_t publish(const struct mex_client *mc, const char *topics, const char *message) {
    char operation[OPERATION_SIZE] = "Publish";
    char *payload = marshaller(operation, topics, message);
    uint8_t ret =  mex_send(mc->sock_fd, payload, strlen(payload));
    free(payload);
    return ret;
}

uint8_t subscribe(const struct mex_client *mc, const char *topics) {
    char operation[OPERATION_SIZE] = "Subscribe";
    char* payload = marshaller(operation, topics, "");
    uint8_t ret = mex_send(mc->sock_fd, payload, strlen(payload));
    free(payload);
    return ret;
}