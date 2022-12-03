#ifndef CAN_SMBUS_SRC_CORE_CS_SEND_ACK_H
#define CAN_SMBUS_SRC_CORE_CS_SEND_ACK_H

#include "io/cs_type.h"
#include "cs_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* CSSendAck_t;


CSSendAck_t CSSendAck_createUser(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg);
CSSendAck_t CSSendAck_createSystem(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg);

void CSSendAck_close(CSSendAck_t obj);

void CSSendAck_send(CSSendAck_t obj, const CSData_t* data);


#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_SEND_ACK_H*/