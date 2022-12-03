#ifndef ODOM_DRIVER_SRC_OD_NAVIGATION_HPP
#define ODOM_DRIVER_SRC_OD_NAVIGATION_HPP

#include <can_smbus/can_smbus.hpp>
#include <std_api/std_api.hpp>
#include "od_type.hpp"

namespace odom_driver
{

class Navigation
{
public:
    virtual Vector_t generate(void) = 0;
};


class NavigationLine : public Navigation
{
private:
    
public:
    NavigationLine(void);

    void NavigationLine_cons(float target_x, float target_y, float target_yaw);

    Vector_t generate(void);
};


class NavigationCircle : public Navigation
{
private:
    
public:
    NavigationCircle(void);

    void NavigationCircle_cons(float r, float theta);

    Vector_t generate(void);
};

}

#endif /*ODOM_DRIVER_SRC_OD_NAVIGATION_HPP*/