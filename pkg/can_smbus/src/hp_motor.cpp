#include "hp_motor.hpp"

namespace can_smbus
{

HPMotor::HPMotor(void)
{
}

HPMotor::HPMotor(int id, DirectRota_t direction, Gateway_t gw_id)
{
    this->HPMotor_cons(id, direction, gw_id);
}

HPMotor::HPMotor(struct hp_motor_param_t* param)
{
    this->HPMotor_cons(param);
}


void HPMotor::HPMotor_cons(int id, DirectRota_t direction, Gateway_t gw_id)
{
    CSType_canId_t driver_id = CSType_createId(CSType_driverType_HPMotor, id);

    this->m_manager = CSManager_create(gw_id, driver_id);

    this->r_power = CSSend_createUser(gw_id, driver_id, 0);
    this->r_acc = CSSendAck_createUser(gw_id, driver_id, 2);

    if(direction == DirectRota_FORWARD)
    {
        this->m_direction_num = 1;
    }else{
        this->m_direction_num = -1;
    }
}

void HPMotor::HPMotor_cons(struct hp_motor_param_t* param)
{
    this->HPMotor_cons(param->id, param->direction, param->gw_id);
}


void HPMotor::init(void)
{
    CSManager_driverStart(this->m_manager);
}

void HPMotor::set_acc(float acc)
{
    CSData_t data_acc = CSData_createFloat(acc);
    CSSend_send(this->r_acc, &data_acc);
}

void HPMotor::set_power(float power)
{
    if(100 <= (int)power)
    {
        power = 100;
    }else if((int)power <= -100){
        power = -100;
    }
    int16_t send_power = (int16_t)(power * 320);
    CSData_t data_power = CSData_createInt16(send_power * this->m_direction_num);
    CSSendAck_send(this->r_power, &data_power);
}

}
