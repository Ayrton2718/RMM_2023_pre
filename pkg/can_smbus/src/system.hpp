#ifndef CAN_SMBUS_SRC_SYSTEM_HPP
#define CAN_SMBUS_SRC_SYSTEM_HPP

#include "type.hpp"

namespace can_smbus
{

void connectionWait(int gw_id = 0);

void safetyOn(void);
void safetyOff(void);

bool isSafety(void);

void allReboot(void);

}

#endif /*CAN_SMBUS_SRC_SYSTEM_HPP*/