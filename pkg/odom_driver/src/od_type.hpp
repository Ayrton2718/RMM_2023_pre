#ifndef ODOM_DRIVER_SRC_OD_TYPE_HPP
#define ODOM_DRIVER_SRC_OD_TYPE_HPP

#include <can_smbus/can_smbus.hpp>
#include <math.h>

namespace odom_driver
{

typedef struct
{
    float x;
    float y;
    float theta;
} Vector_t;

}

#endif /*ODOM_DRIVER_SRC_OD_TYPE_HPP*/