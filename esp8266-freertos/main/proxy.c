#include "proxy.h"

uint8_t publish(const struct mex_client *mc, const char *topic, const char *message) {
    char operation[] = "Publish";
    const size_t buffer_size = strlen(message) + strlen(topic) + OPERATION_SIZE + LITERAL_STRING_SIZE;

    char payload[buffer_size];   
    int b = snprintf(payload, buffer_size, 
    "OP:'%s'\nTHING_ID:'%d'\nTOPICS:['%s']\n%s\n", 
    operation, THING_ID, topic, message);

    if (b > 0 && b < BUFFER_SIZE) {
        return mex_send(mc->sock_fd, payload, strlen(payload));
    } 

    return -1;
}

uint8_t subscribe(const struct mex_client *mc, const char *t) {
    const char op[OPERATION_SIZE] = "Subscribe";

    char i[BUFFER_SIZE];
    
    int b = snprintf(i, BUFFER_SIZE,
    "OP:'%s'\nTHING_ID:'%d'\nTOPICS:['%s']\n%s\n",
    op, THING_ID, t, "");

    if (b > 0 && b < BUFFER_SIZE) {
        return mex_send(mc->sock_fd, i, BUFFER_SIZE);
    } 

    return -1;
}