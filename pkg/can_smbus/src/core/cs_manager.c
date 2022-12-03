#include "cs_manager.h"

#include "io/cs_tx.h"
#include "io/cs_rx.h"
#include "cs_send_ack.h"
#include "cs_checker.h"

struct CSManager_info_t
{
    CSChecker_t checker;

    CSType_gatewayId_t gw_id;
    CSType_canId_t id;
};

CSManager_t CSManager_create(CSType_gatewayId_t gw_id, CSType_canId_t id)
{
    struct CSManager_info_t* _obj = (struct CSManager_info_t*)SAMEM_MALLOC(sizeof(struct CSManager_info_t));
    _obj->checker = CSCHECKER_ID;
    _obj->gw_id = gw_id;
    _obj->id = id;
    return _obj;
}


void CSManager_close(CSManager_t obj)
{
    if(obj != NULL)
    {
        struct CSManager_info_t* _obj = (struct CSManager_info_t*)obj;
        CSCHECKER_CHECK(obj, _obj->checker);
        SAMEM_FREE(obj);
    }
}


void CSManager_driverStart(CSManager_t obj)
{
    struct CSManager_info_t* _obj = (struct CSManager_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);
    CSData_t start_byte;
    start_byte.type = CSData_type_Buff1;
    start_byte.var.Buff[0] = 0x55;

    CSSendAck_t send_ack = CSSendAck_createSystem(_obj->gw_id, _obj->id, CSType_sysDrivReg_StartOrder);
    CSSendAck_send(send_ack, &start_byte);
    CSSendAck_close(send_ack);
}


void CSManager_setMultimuster(CSManager_t obj, CSType_canReg_t mm_reg, CSType_canId_t target_id, CSType_canReg_t target_reg)
{
    struct CSManager_info_t* _obj = (struct CSManager_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);
    
    CSType_canReg_t send_reg;
    switch(mm_reg)
    {
    case 0:
        send_reg = CSType_sysDrivReg_Mmaster0;
        break;

    case 1:
        send_reg = CSType_sysDrivReg_Mmaster1;
        break;

    case 2:
        send_reg = CSType_sysDrivReg_Mmaster2;
        break;

    case 3:
        send_reg = CSType_sysDrivReg_Mmaster3;
        break;

    default:
        SALOG_ERROR("can_smbus", "Invaited reg Number.");
        return;
    }

    CSData_t start_byte;
    start_byte.type = CSData_type_Buff3;
    start_byte.var.Buff[0] = (uint8_t)(target_id >> 8);
    start_byte.var.Buff[1] = (uint8_t)target_id;
    start_byte.var.Buff[2] = (uint8_t)target_reg;

    CSSendAck_t send_ack = CSSendAck_createSystem(_obj->gw_id, _obj->id, send_reg);
    CSSendAck_send(send_ack, &start_byte);
    CSSendAck_close(send_ack);
}

// size_t CSManager_ping(CSManager_t obj, size_t count);