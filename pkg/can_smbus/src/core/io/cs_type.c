#include "cs_type.h"

CSType_canId_t CSType_createId(CSType_driverType_t type, CSType_canId_t dial_id)
{
    return (type << 4) | (dial_id & 0x0F);
}

const char* CSType_driverConvertName(CSType_driverType_t driver_id)
{
    switch(driver_id)
    {
    case CSType_driverType_AbsEncoder:
        return "AbsEncoder";

    case CSType_driverType_Encoder:
        return "Encoder";

    case CSType_driverType_Gyro:
        return "Gyro";
    
    case CSType_driverType_HPMotor:
        return "HPMotor";
    
    case CSType_driverType_LPMotor:
        return "LPMotor";
    
    case CSType_driverType_Servo:
        return "Servo";
    
    case CSType_driverType_Switch:
        return "Switch";
    
    default:
        return "Unknown";
    }
}