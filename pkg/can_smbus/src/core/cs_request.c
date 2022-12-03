#include "cs_request.h"

#include "io/cs_tx.h"
#include "io/cs_rx.h"


struct CSRequest_callbackArg_t
{
    pthread_mutex_t locker;
    pthread_cond_t cond;

    CSType_canReg_t reg;
    size_t len;
    uint8_t hash;
};


static void CSRequest_sendAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, size_t len, const uint8_t* data, CSType_regChannel_t channel);
void CSRequest_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args);


// CSRequest_t CSRequest_createUser(CSType_canId_t id, CSType_canReg_t reg)
// {
//     return CSRequest_create(id, reg, CSType_regChannel_USER);
// }

// CSRequest_t CSRequest_createSystem(CSType_canId_t id, CSType_canReg_t reg)
// {
//     return CSRequest_create(id, reg, CSType_regChannel_SYSTEM);
// }


// static CSRequest_t CSRequest_create(CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel)
// {
//     struct CSRequest_info_t* _obj = (struct CSRequest_info_t*)SAMEM_MALLOC(sizeof(struct CSRequest_info_t));
//     _obj->id = id;
//     _obj->reg = reg;
//     _obj->channel = channel;

//     _obj->arg.reg = reg;
//     _obj->arg.len = 0;
//     _obj->arg.hash = 0;
//     SALOCKER_INIT(&_obj->arg.locker);
//     SACOND_INIT(&_obj->arg.cond);
    
//     CSRx_bindRegAck(id, CSRequest_callback, &_obj->arg, _obj->channel);
//     return _obj;
// }


// void CSRequest_close(CSRequest_t obj)
// {
//     if(obj != NULL)
//     {
//         struct CSRequest_info_t* _obj = (struct CSRequest_info_t*)obj;
//         CSRx_closeReqAck(_obj->id, CSRequest_callback, &_obj->arg, _obj->channel);
//         SACOND_DESTROY(&_obj->arg.cond);
//         SALOCKER_DESTROY(&_obj->arg.locker);
//         SAMEM_FREE(obj);
//     }
// }


void CSRequest_request(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel)
{
    uint8_t data[4];
    CSTx_sendReq(gw_id, id, reg, 0, data, CSTx_priority_Normal, channel);
}

void CSRequest_setModeRequest(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel)
{
    uint8_t len;
    uint8_t data[4];

    len = 1;
    data[0] = CSType_requestMode_Request;
    CSRequest_sendAck(gw_id, id, reg, len, data, channel);
}

void CSRequest_setModeInterval(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint16_t ms, CSType_regChannel_t channel)
{
    uint8_t len;
    uint8_t data[4];

    len = 3;
    data[0] = CSType_requestMode_Interval;
    data[1] = (uint8_t)(ms >> 8);
    data[2] = (uint8_t)ms;

    CSRequest_sendAck(gw_id, id, reg, len, data, channel);
}


static void CSRequest_sendAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, size_t len, const uint8_t* data, CSType_regChannel_t channel)
{
    struct CSRequest_callbackArg_t arg;

    SALOCKER_INIT(&arg.locker, NULL);
    SACOND_INIT(&arg.cond, NULL);
    
    CSRx_bindReqAck(gw_id, id, CSRequest_callback, &arg, channel);

    SABool_t is_hit = SABOOL_FALSE;

    SALOCKER_LOCK(&arg.locker);
    arg.reg = reg;
    arg.len = len;
    arg.hash = 0;
    for(size_t i = 0; i < len; i++)
    {
        arg.hash += data[i];
    }

    for(size_t i = 0; i < 5; i++)
    {
        CSTx_sendReqAck(gw_id, id, reg, len, data, CSTx_priority_Normal, channel);
        struct timespec timeout_spec;
        SATime_timespecAddMs(&timeout_spec, 20);
        if(SACOND_TIMEWAIT(&arg.cond, &arg.locker, &timeout_spec) == 0)
        {
            is_hit = SABOOL_TRUE;
            break;
        }
    }

    SALOCKER_UNLOCK(&arg.locker);

    if(is_hit == SABOOL_FALSE)
    {
        SALOG_ERROR("can_smbus", "Ack can't receive.");
    }

    CSRx_closeReqAck(gw_id, id, CSRequest_callback, &arg, channel);
    SACOND_DESTROY(&arg.cond);
    SALOCKER_DESTROY(&arg.locker);
}


void CSRequest_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args)
{
    struct CSRequest_callbackArg_t* info = (struct CSRequest_callbackArg_t*)args;

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