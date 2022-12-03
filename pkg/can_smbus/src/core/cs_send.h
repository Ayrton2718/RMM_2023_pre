#ifndef CAN_SMBUS_SRC_CORE_CS_SEND_H
#define CAN_SMBUS_SRC_CORE_CS_SEND_H

#include "io/cs_type.h"
#include "cs_data.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* CSSend_t;


CSSend_t CSSend_createUser(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg);
CSSend_t CSSend_createSystem(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg);

void CSSend_close(CSSend_t obj);

void CSSend_send(CSSend_t obj, const CSData_t* data);


#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_SEND_H*/