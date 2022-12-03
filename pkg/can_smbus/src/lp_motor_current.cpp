#include "lp_motor_current.hpp"

namespace can_smbus
{

LPMotorCurrent::LPMotorCurrent(void)
{
}

LPMotorCurrent::LPMotorCurrent(int id, Port_t port, LPMotorCurrentMode_t mode, DirectRota_t direction, Gateway_t gw_id)
{
    this->LPMotorCurrent_cons(id, port, mode, direction);
}


LPMotorCurrent::LPMotorCurrent(struct lp_motor_current_param_t* param)
{
    this->LPMotorCurrent_cons(param);
}

void LPMotorCurrent::LPMotorCurrent_cons(int id, Port_t port, LPMotorCurrentMode_t mode, DirectRota_t direction, Gateway_t gw_id)
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
        this->r_acc = CSSend_createUser(gw_id, driver_id, 2);
        this->r_mode = CSSendAck_createUser(gw_id, driver_id, 4);
        this->r_current = CSSend_createUser(gw_id, driver_id, 6);
        this->r_kp = CSSend_createUser(gw_id, driver_id, 8);
        this->r_ki = CSSend_createUser(gw_id, driver_id, 10);
        this->r_kd = CSSend_createUser(gw_id, driver_id, 12);
        this->r_currenterror = CSSend_createUser(gw_id, driver_id, 14);
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 0);
        break;

    case Port_2:
        this->r_power = CSSend_createUser(gw_id, driver_id, 1);
        this->r_acc = CSSend_createUser(gw_id, driver_id, 3);
        this->r_mode = CSSendAck_createUser(gw_id, driver_id, 5);
        this->r_current = CSSend_createUser(gw_id, driver_id, 7);
        this->r_kp = CSSend_createUser(gw_id, driver_id, 9);
        this->r_ki = CSSend_createUser(gw_id, driver_id, 11);
        this->r_kd = CSSend_createUser(gw_id, driver_id, 13);
        this->r_currenterror = CSSend_createUser(gw_id, driver_id, 15);
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 1);
        break;
    
    default:
        SALOG_ERROR("can_smbus", "LPMotor Unknown Port!, id(0x%03x), port(%d)", id, port);
        return;
    }

    this->m_mode = mode;

    if(direction == DirectRota_FORWARD)
    {
        this->m_direction_num = 1;
    }else{
        this->m_direction_num = -1;
    }
}

void LPMotorCurrent::LPMotorCurrent_cons(struct lp_motor_current_param_t* param)
{
    this->LPMotorCurrent_cons(param->id, param->port, param->mode, param->direction, param->gw_id);
}

void LPMotorCurrent::init(void)
{
    CSData_t data_mode;
    switch (this->m_mode)
    {
    case LPMotorCurrentMode_Voltage:
        data_mode = CSData_createUint8(0);
        CSSendAck_send(this->r_mode, &data_mode);
        break;

    case LPMotorCurrentMode_Current:
        data_mode = CSData_createUint8(1);
        CSSendAck_send(this->r_mode, &data_mode);
        break;

    default:
        SALOG_ERROR("can_smbus", "Encoder Unknown mode!");
        return;
    }

    CSManager_driverStart(this->m_manager);
    CSRegShare_setModeInterval(this->r_var, 10);
}

void LPMotorCurrent::set_acc(float acc)
{
    CSData_t data_acc = CSData_createFloat(acc);
    CSSend_send(this->r_acc, &data_acc);
}

void LPMotorCurrent::set_pid(float kp, float ki, float kd)
{
    CSData_t data_kp = CSData_createFloat(kp);
    CSSend_send(this->r_kp, &data_kp);

    CSData_t data_ki = CSData_createFloat(ki);
    CSSend_send(this->r_ki, &data_ki);

    CSData_t data_kd = CSData_createFloat(kd);
    CSSend_send(this->r_kd, &data_kd);
}

void LPMotorCurrent::set_currenterror(float currenterror)
{
    CSData_t data_currenterror = CSData_createFloat(currenterror);
    CSSend_send(this->r_currenterror, &data_currenterror);
}

void LPMotorCurrent::set_power(float power)
{
    int16_t send_power = (int16_t)(power * 320);
    CSData_t data_power = CSData_createInt16(send_power * this->m_direction_num);
    CSSend_send(this->r_power, &data_power);
}

void LPMotorCurrent::set_current(float current)
{
    CSData_t data_current = CSData_createFloat(current);
    CSSend_send(this->r_current, &data_current);
}

float LPMotorCurrent::getCurrent(void)
{
    CSData_t value = CSRegShare_get(this->r_var);
    if(value.type != CSData_type_Buff4)
    {
        SALOG_ERROR("can_smbus", "none %d", value.type);
        return 0;
    }
    
    return (value.var.Float);
}

}