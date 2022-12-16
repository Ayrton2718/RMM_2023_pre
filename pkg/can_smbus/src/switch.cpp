#include "switch.hpp"

namespace can_smbus
{

Switch::Switch(void)
{
}

Switch::Switch(int id, Port_t port, Interval_t interval, Gateway_t gw_id)
{
    this->Switch_cons(id, port, interval, gw_id);
}

Switch::Switch(const struct switch_param_t* param)
{
    this->Switch_cons(param);
}

void Switch::Switch_cons(int id, Port_t port, Interval_t interval, Gateway_t gw_id)
{
    if(port < 1 || 14 < port)
    {
        SALOG_ERROR("can_smbus", "Invited port.");
        return;
    }

    CSType_canId_t driver_id = CSType_createId(CSType_driverType_Switch, id);

    this->m_manager = CSManager_create(gw_id, driver_id);

    this->r_all_state = CSRegShare_createUser(gw_id, driver_id, 0);

    this->m_interval = interval;
    this->m_port = (uint8_t)port;
}

void Switch::Switch_cons(const struct switch_param_t *param)
{
    this->Switch_cons(param->id, param->port, param->interval, param->gw_id);
}

void Switch::init(void)
{
    CSManager_driverStart(this->m_manager);
    CSRegShare_setModeInterval(this->r_all_state, (uint16_t)this->m_interval);
}

bool Switch::get(void)
{
    CSData_t value = CSRegShare_get(this->r_all_state);
    if(value.type == CSData_type_Buff2)
    {
        return (value.var.Uint16 & (0x01 << (this->m_port - 1))) >> (this->m_port - 1);
    }else{
        SALOG_ERROR("can_smbus", "Invalid data type(%d)", value.type);
    }
    return false;
}

}