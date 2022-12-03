#ifndef CAN_SMBUS_SRC_CORE_CS_ARP_H
#define CAN_SMBUS_SRC_CORE_CS_ARP_H

#include "io/cs_type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct CSArp_data_t
{
    CSType_canId_t id;
    SABool_t is_started;   
};


typedef void* CSArp_t;


CSArp_t CSArp_create(CSType_gatewayId_t gw_id);

void CSArp_close(CSArp_t obj);

void CSArp_printf(CSArp_t obj);

void CSArp_getList(CSArp_t obj, struct CSArp_data_t* list, size_t* list_size);


#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_ARP_H*/