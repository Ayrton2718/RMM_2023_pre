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
    virtual void init() = 0;

    virtual bool is_finish(void) = 0;

    virtual Vector_t finalcoordinates(void) = 0;
    virtual Vector_t generate(void) = 0;
};


class NavigationLine : public Navigation
{
private:
    Vector_t m_target;
    uint64_t m_target_time;

    bool m_finish_flag = false;

    std_api::Timer m_timer;

public:
    NavigationLine(void);
    NavigationLine(float target_x, float target_y, float target_yaw, uint64_t time_ms);

    void NavigationLine_cons(float target_x, float target_y, float target_yaw, uint64_t time_ms);
    
    void init();

    bool is_finish(void);
    
    Vector_t finalcoordinates(void);
    Vector_t generate(void);

    static Navigation* create(float target_x, float target_y, float target_yaw, uint64_t time_ms);
};

}

#endif /*ODOM_DRIVER_SRC_OD_NAVIGATION_HPP*/