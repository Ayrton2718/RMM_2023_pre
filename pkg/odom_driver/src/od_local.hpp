#ifndef ODOM_DRIVER_SRC_OD_LOCAL_HPP
#define ODOM_DRIVER_SRC_OD_LOCAL_HPP

#include <math.h>
#include <can_smbus/can_smbus.hpp>
#include <std_api/std_api.hpp>
#include "od_type.hpp"

namespace odom_driver
{

// localization
class Local
{    
public:
    virtual void init(void) = 0;

    virtual void daemon_start(void) = 0;

    virtual Vector_t estimate(void) = 0;
};

}

#endif /*ODOM_DRIVER_SRC_OD_LOCAL_HPP*/