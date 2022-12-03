#include "rm_system.hpp"

#include "c/rm_m3508.h"

namespace robomas
{

void safetyOn(void)
{
    RMM3508_safetyOn();
}

void safetyOff(void)
{
    RMM3508_safetyOff();
}

}