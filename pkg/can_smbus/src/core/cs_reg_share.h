#ifndef CAN_SMBUS_SRC_CORE_CS_REG_SHARE_H
#define CAN_SMBUS_SRC_CORE_CS_REG_SHARE_H

#include "io/cs_type.h"
#include "cs_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* CSRegShare_t;

CSRegShare_t CSRegShare_createUser(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg);
CSRegShare_t CSRegShare_createSystem(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg);

void CSRegShare_close(CSRegShare_t obj);

void CSRegShare_setModeInterval(CSRegShare_t obj, uint16_t ms);
// void CSRegShare_setModeSetsend(CSRegShare_t obj);
void CSRegShare_setModeRequest(CSRegShare_t obj);

void CSRegShare_request(CSRegShare_t obj);

CSData_t CSRegShare_get(CSRegShare_t obj);


#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_REG_SHARE_H*/