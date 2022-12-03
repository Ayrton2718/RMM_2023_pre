#include "rm_m3508.hpp"

#include "c/rm_m3508.h"

namespace robomas
{

M3508::M3508(Id_t number, Gateway_t gateway)
{
    this->m_number = (int)number;
    this->m_gateway = (size_t)gateway;
    this->m_offset_theta = 0;
}


void M3508::set_conf(float p, float i, float d)
{
    RMM3508_setCoef(this->m_gateway, this->m_number, p, i, d);
}


void M3508::set_advanced_conf(float p, float i, float d, Direction_t direction)
{
    RMM3508_setAdvancedCoef(this->m_gateway, this->m_number, p, i, d, (SABool_t)direction);
}

void M3508::set_pitch(float base_current, float angle)
{
    RMM3508_setPitchAsM3508(this->m_gateway, this->m_number, base_current, angle);
}


void M3508::set_rpm(int16_t rpm)
{
    RMM3508_setRpmAsM3508(this->m_gateway, this->m_number, rpm);
}

void M3508::set_current(float current)
{
    RMM3508_setCurrentAsM3508(this->m_gateway, this->m_number, current);
}


int M3508::get_rpm(void)
{
    return RMM3508_getRpm(this->m_gateway, this->m_number);
}

float M3508::get_real_cur(void)
{
    return RMM3508_getRealCurrentAsM3508(this->m_gateway, this->m_number);
}

float M3508::get_order_cur(void)
{
    return RMM3508_getOrderCurrentAsM3508(this->m_gateway, this->m_number);
}

bool M3508::get_isWakeup(void)
{
    return RMM3508_isWakeup(this->m_gateway, this->m_number);
}

float M3508::get_theta(void)
{
    return RMM3508_getTheta(this->m_gateway, this->m_number) - this->m_offset_theta;
}


void M3508::set_offset(void)
{
    this->m_offset_theta = RMM3508_getTheta(this->m_gateway, this->m_number);
}



M3508Sync::M3508Sync(Id_t number1, Id_t number2, Gateway_t gateway)
{
    this->m_number1 = (int)number1;
    this->m_number2 = (int)number2;
    this->m_gateway = (size_t)gateway;
    this->m_offset_theta = 0;
}

void M3508Sync::set_conf(float p, float i, float d)
{
    RMM3508_setCoef(this->m_gateway, this->m_number1, p, i, d);
    RMM3508_setCoef(this->m_gateway, this->m_number2, p, i, d);
}

void M3508Sync::set_rpm(int16_t rpm)
{
    RMM3508_setRpmSyncAsM3508(this->m_gateway, this->m_number1, this->m_number2, rpm);
}


int M3508Sync::get_rpm(void)
{
    return RMM3508_getRpm(this->m_gateway, this->m_number1);
}

float M3508Sync::get_real_cur(void)
{
    return RMM3508_getRealCurrentAsM3508(this->m_gateway, this->m_number1);
}

float M3508Sync::get_order_cur(void)
{
    return RMM3508_getOrderCurrentAsM3508(this->m_gateway, this->m_number1);
}

bool M3508Sync::get_isWakeup(void)
{
    return RMM3508_isWakeup(this->m_gateway, this->m_number1);
}

float M3508Sync::get_theta(void)
{
    return RMM3508_getTheta(this->m_gateway, this->m_number1) - this->m_offset_theta;
}


void M3508Sync::set_offset(void)
{
    this->m_offset_theta = RMM3508_getTheta(this->m_gateway, this->m_number1);
}

}