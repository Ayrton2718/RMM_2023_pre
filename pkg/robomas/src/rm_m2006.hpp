#ifndef ROBOMAS_SRC_RM_M2006_HPP
#define ROBOMAS_SRC_RM_M2006_HPP

#include <std_api/std_api.hpp>

#include "rm_type.hpp"

namespace robomas
{

class M2006
{
private:
    int m_number;
    size_t m_gateway;
    float m_offset_theta;

public:
    // number : 1 ~ 8
    M2006(Id_t number, Gateway_t gateway = Gateway_1);

    void set_conf(float p, float i, float d);

    void set_rpm(int16_t rpm);
    void set_current(float current);

    int get_rpm(void);
    float get_real_cur(void);
    float get_order_cur(void);
    bool get_isWakeup(void);
    float get_theta(void);

    void set_offset(void);
};

}

#endif /*ROBOMAS_SRC_RM_M2006_HPP*/