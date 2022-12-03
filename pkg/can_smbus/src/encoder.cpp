#include "encoder.hpp"

namespace can_smbus
{

Encoder::Encoder(void)
{
}

Encoder::Encoder(int id, EncoderMode_t mode, DirectRota_t direction, Interval_t interval, EncoderPPR_t ppr, Gateway_t gw_id)
{
    this->Encoder_cons(id, mode, direction, interval, ppr, gw_id);
}

Encoder::Encoder(struct encoder_param_t* param)
{
    this->Encoder_cons(param);
}

void Encoder::Encoder_cons(int id, EncoderMode_t mode, DirectRota_t direction, Interval_t interval, EncoderPPR_t ppr, Gateway_t gw_id)
{
    CSType_canId_t driver_id = CSType_createId(CSType_driverType_Encoder, id);

    this->m_manager = CSManager_create(gw_id, driver_id);

    switch (mode)
    {
    case EncoderMode_Count:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 0);
        break;

    case EncoderMode_Th:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 1);
        break;

    case EncoderMode_Dth:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 2);
        break;
    
    case EncoderMode_Ddth:
        this->r_var = CSRegShare_createUser(gw_id, driver_id, 3);
        break;
    
    default:
        SALOG_ERROR("can_smbus", "Encoder Unknown mode!, id(0x%03x), mode(%d)", id, mode);
        return;
    }

    this->r_ppr = CSSendAck_createUser(gw_id, driver_id, 0);
    this->r_direction = CSSendAck_createUser(gw_id, driver_id, 1);

    this->m_interval = interval;
    this->m_ppr = (uint16_t)ppr;
    this->m_mode = mode;

    if(direction == DirectRota_FORWARD)
    {
        this->m_direction_num = 1;
    }else{
        this->m_direction_num = -1;
    }
}

void Encoder::Encoder_cons(struct encoder_param_t *param)
{
    this->Encoder_cons(param->id, param->mode, param->direction, param->interval, param->ppr, param->gw_id);
}


void Encoder::init(void)
{
    CSManager_driverStart(this->m_manager);

    if(this->m_interval != Interval_DISABLE)
    {
        CSRegShare_setModeInterval(this->r_var, (uint16_t)this->m_interval);
    }

    if(this->m_mode != EncoderMode_Count)
    {
        CSData_t data_ppr = CSData_createUint16(this->m_ppr);
        CSSendAck_send(this->r_ppr, &data_ppr);

        CSData_t data_direction = CSData_createInt8(this->m_direction_num);
        CSSendAck_send(this->r_direction, &data_direction);
    }
}


int32_t Encoder::getAsInt(void)
{
    CSData_t value = CSRegShare_get(this->r_var);
    if(value.type != CSData_type_Buff4)
    {
        SALOG_WARN("can_smbus", "none %d", value.type);
        return 0;
    }

    if(this->m_mode == EncoderMode_Count)
    {
        return value.var.Int32;
    }else{
        return (int32_t)value.var.Float;
    }
}

float Encoder::getAsFloat(void)
{
    CSData_t value = CSRegShare_get(this->r_var);
    if(value.type != CSData_type_Buff4)
    {
        SALOG_WARN("can_smbus", "none %d", value.type);
        return 0;
    }

    if(this->m_mode == EncoderMode_Count)
    {
        return (float)value.var.Int32;
    }else{
        return value.var.Float;
    }
}

}