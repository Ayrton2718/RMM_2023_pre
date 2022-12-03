#include "lp_motor.hpp"

namespace can_smbus
{

LPMotor::LPMotor(void)
{
}

LPMotor::LPMotor(int id, Port_t port, DirectRota_t direction, Gateway_t gw_id)
{
    this->LPMotor_cons(id, port, direction, gw_id);
}


LPMotor::LPMotor(struct lp_motor_param_t* param)
{
    this->LPMotor_cons(param);
}

void LPMotor::LPMotor_cons(int id, Port_t port, DirectRota_t direction, Gateway_t gw_id)
{
    if(!(Port_1 <= port && port <= Port_2))
    {
        SALOG_ERROR("can_smbus", "Invited port.");
        return;
    }

    CSType_canId_t driver_id = CSType_createId(CSType_driverType_LPMotor, id);

    this->m_manager = CSManager_create(gw_id, driver_id);

    switch(port)
    {
    case Port_1:
        this->r_power = CSSend_createUser(gw_id, driver_id, 0);
        this->r_acc = CSSendAck_createUser(gw_id, driver_id, 2);
        break;

    case Port_2:
        this->r_power = CSSend_createUser(gw_id, driver_id, 1);
        this->r_acc = CSSendAck_createUser(gw_id, driver_id, 3);
        break;
    
    default:
        SALOG_ERROR("can_smbus", "LPMotor Unknown Port!, id(0x%03x), port(%d)", id, port);
        return;
    }

    if(direction == DirectRota_FORWARD)
    {
        this->m_direction_num = 1;
    }else{
        this->m_direction_num = -1;
    }
}

void LPMotor::LPMotor_cons(struct lp_motor_param_t* param)
{
    this->LPMotor_cons(param->id, param->port, param->direction, param->gw_id);
}


void LPMotor::init(void)
{
    CSManager_driverStart(this->m_manager);
}

void LPMotor::set_acc(float acc)
{
    CSData_t data_acc = CSData_createFloat(acc);
    CSSendAck_send(this->r_acc, &data_acc);
}

void LPMotor::set_power(float power)
{
    if(100 <= (int)power)
    {
        power = 100;
    }else if((int)power <= -100){
        power = -100;
    }
    int16_t send_power = (int16_t)(power * 320);
    CSData_t data_power = CSData_createInt16(send_power * this->m_direction_num);
    CSSend_send(this->r_power, &data_power);
}

}
