#ifndef CAN_SMBUS_SRC_CORE_CS_REG_SIGNAL_H
#define CAN_SMBUS_SRC_CORE_CS_REG_SIGNAL_H

#include "io/cs_type.h"
#include "cs_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* CSRegSignal_t;

CSRegSignal_t CSRegSignal_createUser(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg);
CSRegSignal_t CSRegSignal_createSystem(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg);

void CSRegSignal_close(CSRegSignal_t obj);

void CSRegSignal_setModeInterval(CSRegSignal_t obj, uint16_t ms);
// void CSRegSignal_setModeSetsend(CSRegSignal_t obj);
void CSRegSignal_setModeRequest(CSRegSignal_t obj);

void CSRegSignal_request(CSRegSignal_t obj);

CSData_t CSRegSignal_wait(CSRegSignal_t obj, CSData_t data, uint64_t timeout);

#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_REG_SIGNAL_H*/