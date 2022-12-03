#ifndef CAN_SMBUS_SRC_CORE_CS_REQUEST_H
#define CAN_SMBUS_SRC_CORE_CS_REQUEST_H

#include "io/cs_type.h"
#include "cs_data.h"

#ifdef __cplusplus
extern "C" {
#endif

// typedef void* CSRequest_t;


// CSRequest_t CSRequest_createUser(CSType_canId_t id, CSType_canReg_t reg);
// CSRequest_t CSRequest_createSystem(CSType_canId_t id, CSType_canReg_t reg);

void CSRequest_request(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel);

void CSRequest_setModeRequest(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel);
void CSRequest_setModeInterval(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint16_t ms, CSType_regChannel_t channel);


#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_REQUEST_H*/