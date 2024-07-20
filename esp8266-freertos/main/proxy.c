#include "proxy.h"
#include "marshaller.h"



struct mex_client create_connection(char *broker_ip, unsigned short int broker_port) {
    struct mex_client mc;
    mc.sock_fd = mex_connect(broker_ip, broker_port);
    mc.st = (mc.sock_fd == -1) ? NOT_CONNECTED : CONNECTED;
    return mc;
}

uint8_t publish(const struct mex_client *mc, const char *topics, const char *message) {
    char operation[OPERATION_SIZE] = "Publish";
    size_t buffer_size = strlen(topics) + strlen(message) + strlen(THING_ID) + OPERATION_SIZE + LITERAL_STRING_SIZE;
    char payload[buffer_size];
    marshaller(operation, topics, message, payload, buffer_size);
    uint8_t ret =  mex_send(mc->sock_fd, payload, strlen(payload));
    return ret;
}

uint8_t subscribe(const struct mex_client *mc, const char *topics) {
    char operation[OPERATION_SIZE] = "Subscribe";
    size_t buffer_size = strlen(topics) + strlen(THING_ID) + OPERATION_SIZE + LITERAL_STRING_SIZE;
    char payload[buffer_size];
    marshaller(operation, topics, "", payload, buffer_size);
    uint8_t ret = mex_send(mc->sock_fd, payload, strlen(payload));
    return ret;
}

uint8_t check_msg(const struct mex_client *mc, char *buffer) {
    return mex_recv(mc->sock_fd, buffer, BUFFER_SIZE);
}