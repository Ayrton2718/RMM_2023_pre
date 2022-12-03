#ifndef CAN_SMBUS_SRC_CORE_IO_CS_TX_H
#define CAN_SMBUS_SRC_CORE_IO_CS_TX_H

#include "cs_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    CSTx_priority_Low = 0,
    CSTx_priority_Normal = 1,
    CSTx_priority_High = 2,
} CSTx_priority_t;

void CSTx_initializer(void);

void CSTx_sendBrc(CSType_gatewayId_t gw_id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority);

void CSTx_sendReg(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority, CSType_regChannel_t channel);
void CSTx_sendRegAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority, CSType_regChannel_t channel);
void CSTx_sendReq(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority, CSType_regChannel_t channel);
void CSTx_sendReqAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority, CSType_regChannel_t channel);

#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_IO_CS_TX_H*/