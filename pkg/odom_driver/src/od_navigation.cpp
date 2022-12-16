#include "od_navigation.hpp"

namespace odom_driver
{

NavigationLine::NavigationLine(void)
{
}

NavigationLine::NavigationLine(float target_x, float target_y, float target_yaw, uint64_t time_ms)
{
    this->NavigationLine_cons(target_x, target_y, target_yaw, time_ms);
}

void NavigationLine::NavigationLine_cons(float target_x, float target_y, float target_yaw, uint64_t time_ms)
{
    m_target.x = target_x;
    m_target.y = target_y;
    m_target.theta = target_yaw;
    m_target_time = time_ms;
}

void NavigationLine::init()
{
    m_timer.start();
}

bool NavigationLine::is_finish(void)
{
    return m_finish_flag;
}

Vector_t NavigationLine::finalcoordinates(void)
{
    Vector_t coordinates;

    coordinates.x = m_target.x * 1.0;
    coordinates.y = m_target.y * 1.0;
    coordinates.theta = m_target.theta * 1.0;

    return coordinates;
}

Vector_t NavigationLine::generate(void)
{
    Vector_t location;

    float tim = (float)m_timer.getMs() / m_target_time;
    if(tim >= 1.0) {
        tim = 1.0;
        this->m_finish_flag = true;
    }

    location.x = m_target.x * tim;
    location.y = m_target.y * tim;
    location.theta = m_target.theta * tim;

    SALOG_INFO("od-navigation", "target_x=%f target_y=%f target_yaw=%f time=%f", location.x, location.y, location.theta, tim);

    return location;
}


Navigation* NavigationLine::create(float target_x, float target_y, float target_yaw, uint64_t time_ms)
{
    return new NavigationLine(target_x, target_y, target_yaw, time_ms);
}

}