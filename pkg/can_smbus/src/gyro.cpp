#include "gyro.hpp"

namespace can_smbus
{

Gyro::Gyro(void)
{
}

Gyro::Gyro(int id, GyroMode_t mode, Interval_t interval, Gateway_t gw_id)
{
    this->Gyro_cons(id, mode, interval, gw_id);
}

Gyro::Gyro(struct gyro_param_t* param)
{
    this->Gyro_cons(param);
}

void Gyro::Gyro_cons(int id, GyroMode_t mode, Interval_t interval, Gateway_t gw_id)
{
    CSType_canId_t driver_id = CSType_createId(CSType_driverType_Gyro, id);

    this->m_manager = CSManager_create(gw_id, driver_id);

    switch(mode)
    {
    case GyroMode_AccX:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 0);
        break;

    case GyroMode_AccY:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 1);
        break;

    case GyroMode_AccZ:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 2);
        break;

    case GyroMode_GyroX:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 3);
        break;

    case GyroMode_GyroY:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 4);
        break;

    case GyroMode_GyroZ:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 5);
        break;

    case GyroMode_Roll:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 6);
        break;

    case GyroMode_Pitch:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 7);
        break;

    case GyroMode_Yaw:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 8);
        break;

    case GyroMode_Quaternion0:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 9);
        break;
    
    case GyroMode_Quaternion1:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 10);
        break;

    case GyroMode_Quaternion2:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 11);
        break;

    case GyroMode_Quaternion3:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 12);
        break;
    
    default:
        SALOG_ERROR("can_smbus", "Gyro Unknown Mode!, id(0x%03x), mode(%d)", id, mode);
        return;
    }

    this->m_interval = interval;
}

void Gyro::Gyro_cons(struct gyro_param_t *param)
{
    this->Gyro_cons(param->id, param->mode, param->interval, param->gw_id);
}

void Gyro::init(void)
{
    CSManager_driverStart(this->m_manager);
    CSRegShare_setModeInterval(this->r_var, (uint16_t)this->m_interval);
}

float Gyro::get(void)
{
    CSData_t value = CSRegShare_get(this->r_var);
    if(value.type != CSData_type_Buff4)
    {
        SALOG_WARN("can_smbus", "none %d", value.type);
        return 0;
    }
    return value.var.Float;
}

}