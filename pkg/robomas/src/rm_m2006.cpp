#include "rm_m2006.hpp"

#include "c/rm_m3508.h"

namespace robomas
{

M2006::M2006(Id_t number, Gateway_t gateway)
{
    this->m_number = (int)number;
    this->m_gateway = (size_t)gateway;
    this->m_offset_theta = 0;
}


void M2006::set_conf(float p, float i, float d)
{
    RMM3508_setCoef(this->m_gateway, this->m_number, p, i, d);
}


void M2006::set_rpm(int16_t rpm)
{
    RMM3508_setRpmAsM2006(this->m_gateway, this->m_number, rpm);
}

void M2006::set_current(float current)
{
    RMM3508_setCurrentAsM2006(this->m_gateway, this->m_number, current);
}


int M2006::get_rpm(void)
{
    return RMM3508_getRpm(this->m_gateway, this->m_number);
}

float M2006::get_real_cur(void)
{
    return RMM3508_getRealCurrentAsM2006(this->m_gateway, this->m_number);
}

float M2006::get_order_cur(void)
{
    return RMM3508_getOrderCurrentAsM2006(this->m_gateway, this->m_number);
}

bool M2006::get_isWakeup(void)
{
    return RMM3508_isWakeup(this->m_gateway, this->m_number);
}

float M2006::get_theta(void)
{
    return RMM3508_getTheta(this->m_gateway, this->m_number) - this->m_offset_theta;
}


void M2006::set_offset(void)
{
    this->m_offset_theta = RMM3508_getTheta(this->m_gateway, this->m_number);
}

}