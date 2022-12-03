#ifndef CAN_SMBUS_SRC_CORE_CS_CHECKER_H
#define CAN_SMBUS_SRC_CORE_CS_CHECKER_H

#include "io/cs_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CSCHECKER_ID (0x429A67F3)

typedef uint32_t CSChecker_t;

#define CSCHECKER_CHECK(obj, checker) if(obj == NULL | checker != CSCHECKER_ID){SALOG_ERROR_NOFMT("can_smbus", "Uninitialized obj");}

#ifdef __cplusplus
}
#endif


#endif /*CAN_SMBUS_SRC_CORE_CS_CHECKER_H*/