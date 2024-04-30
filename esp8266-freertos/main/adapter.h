#ifndef ADAPTER_H
#define ADAPTER_H

#include <lwip/sys.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/err.h>


struct adapter {
    int sock_fd;
};

struct adapter adapter_init(char *managing_system_host, unsigned short int managing_system_port);
uint8_t submit_data(const struct adapter *ad, int fluid_volume, int reservoir_capacity, int temperature, int battery);
uint8_t adapt(uint8_t sock_fd);

#endif // ADAPTER_H