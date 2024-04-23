#include "proxy.h"
#include "marshaller.h"

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