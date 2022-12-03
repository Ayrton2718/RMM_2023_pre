#include "abs_encoder.hpp"

namespace can_smbus
{

AbsEncoder::AbsEncoder(void)
{
}

AbsEncoder::AbsEncoder(int id, DirectRota_t direction, Interval_t interval, Gateway_t gw_id)
{
    this->AbsEncoder_cons(id, direction, interval, gw_id);
}

AbsEncoder::AbsEncoder(struct abs_encoder_param_t* param)
{
    this->AbsEncoder_cons(param);
}


void AbsEncoder::AbsEncoder_cons(int id, DirectRota_t direction, Interval_t interval, Gateway_t gw_id)
{
    CSType_canId_t driver_id = CSType_createId(CSType_driverType_AbsEncoder, id);

    this->m_manager = CSManager_create((CSType_gatewayId_t)gw_id, driver_id);

    this->r_var = CSRegShare_createUser((CSType_gatewayId_t)gw_id, driver_id, 0);

    this->m_interval = interval;

    if(direction == DirectRota_FORWARD)
    {
        this->m_direction_num = 1;
    }else{
        this->m_direction_num = -1;
    }
}

void AbsEncoder::AbsEncoder_cons(struct abs_encoder_param_t *param)
{
    this->AbsEncoder_cons(param->id, param->direction, param->interval, param->gw_id);
}


void AbsEncoder::init(void)
{
    CSManager_driverStart(this->m_manager);

    if(this->m_interval != Interval_DISABLE)
    {
        CSRegShare_setModeInterval(this->r_var, (uint16_t)this->m_interval);
    }
}


float AbsEncoder::angle(void)
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