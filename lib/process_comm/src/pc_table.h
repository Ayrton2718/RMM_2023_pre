#ifndef PC_TABLE_H
#define PC_TABLE_H

#include <std_api/std_api.h>

#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif


void PCTable_init(void);

void PCTable_set(const char* service_type, const char* name, const char* ip, uint16_t port);


typedef void* PCTable_instance_t;
PCTable_instance_t PCTable_loockUp(const char* service_type, const char* name);
struct sockaddr_in PCTable_getIp(PCTable_instance_t instance);

void PCTable_setSockaddr(const char* service_type, const char* name, const struct sockaddr_in* addr);
void PCTable_deactivate(const char* service_type, const char* name, const struct sockaddr_in* addr);

#ifdef __cplusplus
}
#endif

#endif /*PC_TABLE_H*/