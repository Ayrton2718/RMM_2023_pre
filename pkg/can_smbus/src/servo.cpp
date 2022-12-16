#include "servo.hpp"

namespace can_smbus
{

Servo::Servo(void)
{
}

Servo::Servo(int id, Port_t port, Gateway_t gw_id)
{
    this->Servo_cons(id, port, gw_id);
}

Servo::Servo(const struct servo_param_t* param)
{
    this->Servo_cons(param);
}


void Servo::Servo_cons(int id, Port_t port, Gateway_t gw_id)
{
    if(!(Port_1 <= port && port <= Port_4))
    {
        SALOG_ERROR("can_smbus", "Invited port.");
        return;
    }

    CSType_canId_t driver_id = CSType_createId(CSType_driverType_Servo, id);

    this->m_manager = CSManager_create(gw_id, driver_id);

    switch(port)
    {
    case Port_1:
        this->r_angle = CSSend_createUser(gw_id, driver_id, 0);
        this->r_enable = CSSendAck_createUser(gw_id, driver_id, 4);
        break;

    case Port_2:
        this->r_angle = CSSend_createUser(gw_id, driver_id, 1);
        this->r_enable = CSSendAck_createUser(gw_id, driver_id, 5);
        break;

    case Port_3:
        this->r_angle = CSSend_createUser(gw_id, driver_id, 2);
        this->r_enable = CSSendAck_createUser(gw_id, driver_id, 6);
        break;

    case Port_4:
        this->r_angle = CSSend_createUser(gw_id, driver_id, 3);
        this->r_enable = CSSendAck_createUser(gw_id, driver_id, 7);
        break;
    
    default:
        SALOG_ERROR("can_smbus", "LPMotor Unknown Port!, id(0x%03x), port(%d)", id, port);
        return;
    }
}

void Servo::Servo_cons(const struct servo_param_t *param)
{
    this->Servo_cons(param->id, param->port, param->gw_id);
}


void Servo::init(void)
{
    CSManager_driverStart(this->m_manager);
}


void Servo::enable(void)
{
    CSData_t data_acc = CSData_createUint8(1);
    CSSendAck_send(this->r_enable, &data_acc);
}

void Servo::disable(void)
{
    CSData_t data_acc = CSData_createUint8(0);
    CSSendAck_send(this->r_enable, &data_acc);
}


void Servo::set_angle(float angle)
{
    CSData_t data_acc = CSData_createFloat(angle);
    CSSend_send(this->r_angle, &data_acc);
}

}