#ifndef PC_MDNS_H
#define PC_MDNS_H

#include "pc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* PCMdns_register_t;

typedef enum
{
    PCMdns_protocol_Udp,
    PCMdns_protocol_Tcp
} PCMdns_protocol_t;

typedef struct
{
    struct sockaddr_in addr;
} PCMdns_server_t;


int PCMdns_createSocket(PCMdns_protocol_t protocol, struct sockaddr_in* addr_out);

// PCMdns_register_t PCMdns_registServer(int sock, const struct sockaddr_in* addr_in, PCMdns_protocol_t protocol, const char* service_name, const char* name, CC_obj param);
// void PCMdns_close(PCMdns_register_t service);

// SABool_t PCMdns_browseServer(PCMdns_protocol_t protocol, const char* service_name, const char* name, PCMdns_server_t* res);
// SABuffer_t PCMdns_browseServerAll(PCMdns_protocol_t protocol, const char* service_name, const char* name);

// void PCMdns_sendtoServer(int sock, PCMdns_server_t server);

#ifdef __cplusplus
}
#endif

#endif /*PC_MDNS_H*/