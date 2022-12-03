#ifndef CAN_SMBUS_SRC_CORE_CS_SYSTEM_H
#define CAN_SMBUS_SRC_CORE_CS_SYSTEM_H

#include "io/cs_type.h"
#include "cs_data.h"

#ifdef __cplusplus
extern "C" {
#endif

void CSSystem_initializer(void);

void CSSystem_connectionWait(size_t gw_id);

void CSSystem_safetyOn(void);
void CSSystem_safetyOff(void);
SABool_t CSSystem_isSafety(void);

void CSSystem_allReboot(void);


#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_SYSTEM_H*/