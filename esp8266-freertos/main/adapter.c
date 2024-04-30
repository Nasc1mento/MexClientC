#include "adapter.h"


// b'ADAPT\nThing:1-new-adryan\nfluid_volume:800.0\nreservoir_capacity:1000.0\ntemperature:30\nbattery:100'


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

uint8_t submit_data(const struct adapter *ad, int fluid_volume, int reservoir_capacity, int temperature, int battery) {
    char buffer[128];
    snprintf(buffer, sizeof buffer, "ADAPT\nThing:1-new-adryan\nfluid_volume:%d\nreservoir_capacity:%d\ntemperature:%d\nbattery:%d\n", fluid_volume, reservoir_capacity, temperature, battery);
    return send(ad->sock_fd, buffer, sizeof buffer, 0) == sizeof buffer ? 0 : -1;
}

uint8_t adapt(uint8_t sock_fd) {
    char buffer[128];
    ssize_t b = recv(sock_fd, buffer, sizeof buffer - 1, 0);

    if (b <= 0) {
        close(sock_fd);
        return -1;
    }

    buffer[b] = '\0';

    int loop_interval = 0;

    char *loop_interval_str = strstr(buffer, "loop_interval");
    if (loop_interval_str) {
        loop_interval_str += strlen("loop_interval");
        loop_interval_str = strchr(loop_interval_str, ':');
        if (loop_interval_str) {
            loop_interval_str++;
            loop_interval = atoi(loop_interval_str);
        }
    }

    printf(buffer);

    return loop_interval;
}