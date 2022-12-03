#ifndef CAN_SMBUS_SRC_TYPE_HPP
#define CAN_SMBUS_SRC_TYPE_HPP

namespace can_smbus
{

// Direction of rotation
typedef enum
{
    DirectRota_FORWARD = 0,
    DirectRota_INVERSE = 1,
} DirectRota_t;

typedef enum
{
    Interval_DISABLE = 0,
    Interval_1Hz = 1000,
    Interval_2Hz = 500,
    Interval_5Hz = 200,
    Interval_10Hz = 100,
    Interval_20Hz = 50,
    Interval_50Hz = 20,
    Interval_100Hz = 10,
} Interval_t;

typedef enum
{
    Port_1 = 1,
    Port_2 = 2,
    Port_3 = 3,
    Port_4 = 4,
    Port_5 = 5,
    Port_6 = 6,
    Port_7 = 7,
    Port_8 = 8
} Port_t;

// Direction of rotation
typedef enum
{
    Gateway_1 = 0,
    Gateway_2 = 1,
    Gateway_3 = 2,
    Gateway_4 = 3
} Gateway_t;

}

#endif /*CAN_SMBUS_SRC_TYPE_HPP*/