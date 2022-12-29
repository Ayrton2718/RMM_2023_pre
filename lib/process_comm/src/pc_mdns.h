#ifndef PC_MDNS_H
#define PC_MDNS_H

#include "pc_type.h"
#include "pc_table.h"

#ifdef __cplusplus
extern "C" {
#endif

void PCMdns_init(void);

typedef enum
{
    PCMdnsProt_Ipv4Udp,
    PCMdnsProt_Ipv6Udp,
    PCMdnsProt_Ipv4Tcp,
    PCMdnsProt_Ipv6Tcp
} PCMdnsProt_t;

int PCMdns_createSocket(PCMdnsProt_t protocol, struct sockaddr_in* addr_out);


typedef void* PCMdnsRegister_t;

PCMdnsRegister_t PCMdnsRegister_create(int sock, const struct sockaddr_in* addr_in, const char* service_name, const char* name);
void PCMdnsRegister_close(PCMdnsRegister_t service);


// PCTable_info_t PCMdnsServer_browse(const char* service_name, const char* name);
// void PCMdnsServer_sendto(int sock, PCTable_info_t server, const void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /*PC_MDNS_H*/