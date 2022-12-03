#ifndef ROBOMAS_SRC_RM_M3508_HPP
#define ROBOMAS_SRC_RM_M3508_HPP

#include <std_api/std_api.hpp>

#include "rm_type.hpp"

namespace robomas
{

typedef enum
{
    Direction_Forward = SABOOL_FALSE,
    Direction_Inverse = SABOOL_TRUE,
} Direction_t;

class M3508
{
private:
    int m_number;
    size_t m_gateway;
    float m_offset_theta;

public:
    // number : 1 ~ 8
    M3508(Id_t number, Gateway_t gateway = Gateway_1);

    void set_conf(float p, float i, float d);
    void set_advanced_conf(float p, float i, float d, Direction_t direction);

    void set_rpm(int16_t rpm);
    void set_current(float current);
    void set_pitch(float base_current, float angle);

    int get_rpm(void);
    float get_real_cur(void);
    float get_order_cur(void);
    bool get_isWakeup(void);
    float get_theta(void);

    void set_offset(void);
};

class M3508Sync
{
private:
    int m_number1;
    int m_number2;
    size_t m_gateway;
    float m_offset_theta;

public:
    // number : 1 ~ 8
    M3508Sync(Id_t number1, Id_t number2, Gateway_t gateway = Gateway_1);

    void set_conf(float p, float i, float d);

    void set_rpm(int16_t rpm);

    int get_rpm(void);
    float get_real_cur(void);
    float get_order_cur(void);
    bool get_isWakeup(void);
    float get_theta(void);

    void set_offset(void);
};

}

#endif /*ROBOMAS_SRC_RM_M3508_HPP*/