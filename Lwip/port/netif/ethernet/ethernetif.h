#ifndef ETHERNETIF_H
#define ETHERNETIF_H

#include "err.h"
#include "netif.h"
#include "drv.h"

#ifndef IFXETH_RTX_BUFFER_SIZE
#define IFXETH_RTX_BUFFER_SIZE 1536
#endif

#ifdef __cplusplus
extern "C" {
#endif

err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);
void ethernetif_recv(struct netif *netif);
void ethernetif_poll(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif 
