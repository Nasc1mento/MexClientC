#include "adapter.h"


struct adapter adapter_init(char *managing_system_host, unsigned short int managing_system_port) {
    char addr_str[128];
    struct sockaddr_in broker_info;
    struct adapter ad;

    broker_info.sin_addr.s_addr = inet_addr(managing_system_host);
    broker_info.sin_family = AF_INET;

    broker_info.sin_port = htons(managing_system_port);
    inet_ntoa_r(broker_info.sin_addr, addr_str, sizeof(addr_str) - 1);

    ad.sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (ad.sock_fd < 0) {
        close(ad.sock_fd);
    }

    if (connect(ad.sock_fd, (struct sockaddr *)&broker_info, sizeof(broker_info)) != 0) {
        close(ad.sock_fd);
    } 

    return ad;
}

uint8_t submit_data(const struct adapter *ad, int num_args, ...) {
    
    va_list valist;
    va_start(valist, num_args);

    char buffer[128];
    char *buffer_ptr = buffer;

    buffer_ptr += sprintf(buffer_ptr, "ADAPT\nThing:%s\n", THING_ID);

    for (int i = 0; i < num_args; i++) {
        char *key = va_arg(valist, char *);
        char *value = va_arg(valist, char *);

        buffer_ptr += sprintf(buffer_ptr, "%s:%s\n", key, value);
    }
    
    // b'ADAPT\nThing:1-new-adryan\nfluid_volume:800.0\nreservoir_capacity:1000.0\ntemperature:30\nbattery:100'

    va_end(valist);

    printf("%s", buffer);

    return send(ad->sock_fd, buffer, strlen(buffer), 0) == strlen(buffer) ? 0 : 1;
}

char* adapt(uint8_t sock_fd, char *param) {
    char buffer[128];
    ssize_t b = recv(sock_fd, buffer, sizeof buffer - 1, 0);

    if (b <= 0) {
        close(sock_fd);
        return -1;
    }

    buffer[b] = '\0';

    char *response = strstr(buffer, param);

    if (response) {
        response += strlen(param);
        response = strchr(response, ':');
        if (response) {
            response++;
        }
    }

    printf("%s", buffer);

    return response;
}