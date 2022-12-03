/*
    A basis of receive system.
*/
#ifndef CAN_SMBUS_SRC_CORE_IO_CS_RX_H
#define CAN_SMBUS_SRC_CORE_IO_CS_RX_H

#include "cs_type.h"

#ifdef __cplusplus
extern "C" {
#endif

// callback function's type.
typedef void (*CSRx_callback_t)(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args);

void CSRx_initializer(void);

// You can set the callback function with a id and reg.
// If it receive a data that is the same id and reg, the callback will be called.
void CSRx_bindReg(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSRx_callback_t callback, void* args, CSType_regChannel_t channel);
void CSRx_closeReg(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSRx_callback_t callback, void* args, CSType_regChannel_t channel);

void CSRx_bindRegAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSRx_callback_t callback, void* args, CSType_regChannel_t channel);
void CSRx_closeRegAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSRx_callback_t callback, void* args, CSType_regChannel_t channel);
void CSRx_bindReqAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSRx_callback_t callback, void* args, CSType_regChannel_t channel);
void CSRx_closeReqAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSRx_callback_t callback, void* args, CSType_regChannel_t channel);

void CSRx_bindBrc(CSType_gatewayId_t gw_id, CSType_canReg_t reg, CSRx_callback_t callback, void* args);
void CSRx_closeBrc(CSType_gatewayId_t gw_id, CSType_canReg_t reg, CSRx_callback_t callback, void* args);

#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_IO_CS_RX_H*/