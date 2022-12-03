#include "system.hpp"

#include "core/cs_system.h"

namespace can_smbus
{

void connectionWait(int gw_id)
{
    CSSystem_connectionWait(gw_id);
}

void safetyOn(void)
{
    CSSystem_safetyOn();
}

void safetyOff(void)
{
    CSSystem_safetyOff();
}

bool isSafety(void)
{
    return CSSystem_isSafety();
}

void allReboot(void)
{
    CSSystem_allReboot();
}


}