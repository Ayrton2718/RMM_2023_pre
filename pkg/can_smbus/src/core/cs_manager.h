#ifndef CAN_SMBUS_SRC_CORE_CS_MANAGER_H
#define CAN_SMBUS_SRC_CORE_CS_MANAGER_H

#include "io/cs_type.h"
#include "cs_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* CSManager_t;

CSManager_t CSManager_create(CSType_gatewayId_t gw_id, CSType_canId_t id);

void CSManager_close(CSManager_t obj);

void CSManager_driverStart(CSManager_t obj);

void CSManager_setMultimuster(CSManager_t obj, CSType_canReg_t mm_reg, CSType_canId_t target_id, CSType_canReg_t target_reg);

// size_t CSManager_ping(CSManager_t obj, size_t count);


#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_MANAGER_H*/