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
uint8_t submit_data(const struct adapter *ad, int num_args, ...);
char* adapt(uint8_t sock_fd, char *param);

#endif // ADAPTER_H