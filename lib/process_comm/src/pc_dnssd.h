#ifdef __APPLE__

#ifndef PC_DNSSD_H
#define PC_DNSSD_H

#include "pc_type.h"
#include "pc_mdns_type.h"
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

void PCDnssd_init(void);

void* PCDnssd_registServer(int sock, const struct sockaddr_in* addr_in, const char* type_name, const char* name);
void PCDnssd_closeServer(void* reg);


#ifdef __cplusplus
}
#endif

#endif /*PC_DNSSD_H*/

#endif /*__APPLE__*/
