#include "cs_send_ack.h"

#include "io/cs_tx.h"
#include "io/cs_rx.h"
#include "cs_checker.h"

struct CSSendAck_callbackArg_t
{
    pthread_mutex_t locker;
    pthread_cond_t cond;

    CSType_canReg_t reg;
    size_t len;
    uint8_t hash;
};

struct CSSendAck_info_t
{
    CSChecker_t checker;

    CSType_gatewayId_t gw_id;
    CSType_canId_t id;
    CSType_canReg_t reg;
    CSType_regChannel_t channel;

    struct CSSendAck_callbackArg_t arg;
};


static CSSendAck_t CSSendAck_create(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel);
void CSSendAck_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args);


CSSendAck_t CSSendAck_createUser(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg)
{
    return CSSendAck_create(gw_id, id, reg, CSType_regChannel_USER);
}

CSSendAck_t CSSendAck_createSystem(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg)
{
    return CSSendAck_create(gw_id, id, reg, CSType_regChannel_SYSTEM);
}


static CSSendAck_t CSSendAck_create(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel)
{
    struct CSSendAck_info_t* _obj = (struct CSSendAck_info_t*)SAMEM_MALLOC(sizeof(struct CSSendAck_info_t));

    _obj->checker = CSCHECKER_ID;

    _obj->gw_id = gw_id;
    _obj->id = id;
    _obj->reg = reg;
    _obj->channel = channel;

    _obj->arg.reg = reg;
    _obj->arg.len = 0;
    _obj->arg.hash = 0;
    SALOCKER_INIT(&_obj->arg.locker, NULL);
    SACOND_INIT(&_obj->arg.cond, NULL);
    
    CSRx_bindRegAck(gw_id, id, CSSendAck_callback, &_obj->arg, _obj->channel);
    return _obj;
}


void CSSendAck_close(CSSendAck_t obj)
{
    if(obj != NULL)
    {
        struct CSSendAck_info_t* _obj = (struct CSSendAck_info_t*)obj;
        CSCHECKER_CHECK(obj, _obj->checker);

        CSRx_closeRegAck(_obj->gw_id, _obj->id, CSSendAck_callback, &_obj->arg, _obj->channel);
        SACOND_DESTROY(&_obj->arg.cond);
        SALOCKER_DESTROY(&_obj->arg.locker);
        SAMEM_FREE(obj);
    }
}


void CSSendAck_send(CSSendAck_t obj, const CSData_t* data)
{
    struct CSSendAck_info_t* _obj = (struct CSSendAck_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    size_t len = CSData_getSize(data);
    const uint8_t* buff = data->var.Buff;

    SABool_t is_hit = SABOOL_FALSE;

    SALOCKER_LOCK(&_obj->arg.locker);
    _obj->arg.len = len;
    _obj->arg.hash = 0;
    for(size_t i = 0; i < len; i++)
    {
        _obj->arg.hash += buff[i];
    }

    for(size_t i = 0; i < 5; i++)
    {
        CSTx_sendRegAck(_obj->gw_id, _obj->id, _obj->reg, len, buff, CSTx_priority_Normal, _obj->channel);
        struct timespec timeout_spec;
        SATime_timespecAddMs(&timeout_spec, 20);
        if(SACOND_TIMEWAIT(&_obj->arg.cond, &_obj->arg.locker, &timeout_spec) == 0)
        {
            is_hit = SABOOL_TRUE;
            break;
        }
    }
    SALOCKER_UNLOCK(&_obj->arg.locker);

    if(is_hit == SABOOL_FALSE)
    {
        SALOG_ERROR("can_smbus", "Ack can't receive.");
    }
}


void CSSendAck_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args)
{
    struct CSSendAck_callbackArg_t* info = (struct CSSendAck_callbackArg_t*)args;
    
    if(len == 2)
    {
        SALOCKER_LOCK(&info->locker);
        if(CSTYPE_GET_NUMBER_REG(data[0]) == info->reg)
        {
            if(data[1] == info->hash)
            {
                SACOND_SIGNAL(&info->cond);
            }
        }
        SALOCKER_UNLOCK(&info->locker);
    }
}