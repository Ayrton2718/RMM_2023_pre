#include "cs_reg_share.h"

#include "io/cs_rx.h"
#include "io/cs_tx.h"
#include "cs_request.h"
#include "cs_checker.h"

struct CSRegShare_callbackArg_t
{
    SABool_t is_revers;
    uint8_t len[2];
    uint8_t buff[2][4];
};

struct CSRegShare_info_t
{
    CSChecker_t checker;
    CSType_gatewayId_t gw_id;
    CSType_canId_t id;
    CSType_canReg_t reg;
    CSType_regChannel_t channel;
    struct CSRegShare_callbackArg_t  arg;
};

static CSRegShare_t CSRegShare_create(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel);
void CSRegShare_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args);


CSRegShare_t CSRegShare_createUser(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg)
{
    return CSRegShare_create(gw_id, id, reg, CSType_regChannel_USER);
}

CSRegShare_t CSRegShare_createSystem(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg)
{
    return CSRegShare_create(gw_id, id, reg, CSType_regChannel_SYSTEM);
}

static CSRegShare_t CSRegShare_create(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel)
{
    struct CSRegShare_info_t* _obj = (struct CSRegShare_info_t*)SAMEM_MALLOC(sizeof(struct CSRegShare_info_t));

    _obj->checker = CSCHECKER_ID;

    _obj->gw_id = gw_id;
    _obj->id = id;
    _obj->reg = reg;
    _obj->channel = channel;

    _obj->arg.is_revers = 0;
    _obj->arg.len[0] = 0;
    _obj->arg.len[1] = 0;
    memset(_obj->arg.buff, 0x00, sizeof(_obj->arg.buff));
    
    CSRx_bindReg(gw_id, id, reg, CSRegShare_callback, &_obj->arg, channel);

    return _obj; 
}


void CSRegShare_close(CSRegShare_t obj)
{
    if(obj != NULL)
    {
        struct CSRegShare_info_t* _obj = (struct CSRegShare_info_t*)obj;
        CSCHECKER_CHECK(obj, _obj->checker);
        
        CSRx_closeReg(_obj->gw_id, _obj->id, _obj->reg, CSRegShare_callback, &_obj->arg, _obj->channel);
        SAMEM_FREE(obj);
    }
}


void CSRegShare_setModeInterval(CSRegShare_t obj, uint16_t ms)
{
    struct CSRegShare_info_t* _obj = (struct CSRegShare_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    if(_obj->channel == CSType_regChannel_USER)
    {
        CSRequest_setModeInterval(_obj->gw_id, _obj->id, _obj->reg, ms, _obj->channel);
    }else{
        SALOG_ERROR("can_smbus", "This mode isn't enabled in this type(%d)", _obj->channel);
    }
}

// void CSRegShare_setModeSetsend(CSRegShare_t obj)
// {
//     struct CSRegShare_info_t* _obj = (struct CSRegShare_info_t*)obj;
//     CSTx_userModeSetsend(_obj->id, _obj->reg, CSTx_priority_Normal);
// }

void CSRegShare_setModeRequest(CSRegShare_t obj)
{
    struct CSRegShare_info_t* _obj = (struct CSRegShare_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    if(_obj->channel == CSType_regChannel_USER)
    {
        CSRequest_setModeRequest(_obj->gw_id, _obj->id, _obj->reg, _obj->channel);
    }else{
        SALOG_ERROR("can_smbus", "This mode isn't enabled in this type(%d)", _obj->channel);
    }
}


void CSRegShare_request(CSRegShare_t obj)
{
    struct CSRegShare_info_t* _obj = (struct CSRegShare_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);
    
    CSRequest_request(_obj->gw_id, _obj->id, _obj->reg, _obj->channel);
}

CSData_t CSRegShare_get(CSRegShare_t obj)
{
    struct CSRegShare_info_t* _obj = (struct CSRegShare_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    struct CSRegShare_callbackArg_t arg;
    memcpy(&arg, &_obj->arg, sizeof(struct CSRegShare_callbackArg_t));

    CSData_t return_data;
    return_data.type = arg.len[arg.is_revers];
    memcpy(return_data.var.Buff, arg.buff[arg.is_revers], sizeof(uint8_t) * 4);

    return return_data;
}


void CSRegShare_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args)
{
    struct CSRegShare_callbackArg_t* buff_info = (struct CSRegShare_callbackArg_t*)args;
    
    SABool_t next_revers = SABOOL_NOT(buff_info->is_revers);
    buff_info->len[next_revers] = len;
    memcpy(buff_info->buff[next_revers], data, sizeof(uint8_t) * 4);
    buff_info->is_revers = next_revers;

    // SALOG_INFO("can_smbus", "Regshare get %d [0x%02x, 0x%02x, 0x%02x, 0x%02x]", len, data[0], data[1], data[2], data[3]);
}
