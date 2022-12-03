#include "cs_reg_signal.h"

#include "io/cs_rx.h"
#include "io/cs_tx.h"
#include "cs_request.h"
#include "cs_checker.h"


struct CSRegSignal_data_t
{
    uint8_t len;
    uint8_t buff[4];
};

struct CSRegSignal_callbackArg_t
{
    pthread_mutex_t locker;
    pthread_cond_t cond;

    SAQueue_t queue;
    struct CSRegSignal_data_t buffer[8];
};

struct CSRegSignal_info_t
{
    CSChecker_t checker;

    CSType_gatewayId_t gw_id;
    CSType_canId_t id;
    CSType_canReg_t reg;
    CSType_regChannel_t channel;
    struct CSRegSignal_callbackArg_t     arg;
};

static CSRegSignal_t CSRegSignal_create(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel);
void CSRegSignal_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args);


CSRegSignal_t CSRegSignal_createUser(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg)
{
    return CSRegSignal_create(gw_id, id, reg, CSType_regChannel_USER);
}

CSRegSignal_t CSRegSignal_createSystem(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg)
{
    return CSRegSignal_create(gw_id, id, reg, CSType_regChannel_SYSTEM);
}

static CSRegSignal_t CSRegSignal_create(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSType_regChannel_t channel)
{
    struct CSRegSignal_info_t* _obj = (struct CSRegSignal_info_t*)SAMEM_MALLOC(sizeof(struct CSRegSignal_info_t));

    _obj->checker = CSCHECKER_ID;

    _obj->gw_id = gw_id;
    _obj->id = id;
    _obj->reg = reg;
    _obj->channel = channel;

    SALOCKER_INIT(&_obj->arg.locker, NULL);
    SACOND_INIT(&_obj->arg.cond, NULL);
    SAQueue_init(&_obj->arg.queue, sizeof(struct CSRegSignal_data_t), 8, &_obj->arg.buffer);

    CSRx_bindReg(gw_id, id, reg, CSRegSignal_callback, &_obj->arg, channel);
    return _obj;
}


void CSRegSignal_close(CSRegSignal_t obj)
{
    if(obj != NULL)
    {
        struct CSRegSignal_info_t* _obj = (struct CSRegSignal_info_t*)obj;
        CSCHECKER_CHECK(obj, _obj->checker);
        
        CSRx_closeReg(_obj->gw_id, _obj->id, _obj->reg, CSRegSignal_callback, &_obj->arg, _obj->channel);
        SALOCKER_DESTROY(&_obj->arg.locker);
        SACOND_DESTROY(&_obj->arg.cond);
        SAMEM_FREE(obj);
    }
}


void CSRegSignal_setModeInterval(CSRegSignal_t obj, uint16_t ms)
{
    struct CSRegSignal_info_t* _obj = (struct CSRegSignal_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);
    
    if(_obj->channel == CSType_regChannel_USER)
    {
        CSRequest_setModeInterval(_obj->gw_id, _obj->id, _obj->reg, ms, _obj->channel);
    }else{
        SALOG_ERROR("can_smbus", "This mode isn't enabled in this type(%d)", _obj->channel);
    }
}

// void CSRegSignal_setModeSetsend(CSRegSignal_t obj)
// {
//     struct CSRegSignal_info_t* _obj = (struct CSRegSignal_info_t*)obj;
//     CSTx_userModeSetsend(_obj->id, _obj->reg, CSTx_priority_Normal);
// }

void CSRegSignal_setModeRequest(CSRegSignal_t obj)
{
    struct CSRegSignal_info_t* _obj = (struct CSRegSignal_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    if(_obj->channel == CSType_regChannel_USER)
    {
        CSRequest_setModeRequest(_obj->gw_id, _obj->id, _obj->reg, _obj->channel);
    }else{
        SALOG_ERROR("can_smbus", "This mode isn't enabled in this type(%d)", _obj->channel);
    }
}

void CSRegSignal_request(CSRegSignal_t obj)
{
    struct CSRegSignal_info_t* _obj = (struct CSRegSignal_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    CSRequest_request(_obj->gw_id, _obj->id, _obj->reg, _obj->channel);
}

CSData_t CSRegSignal_wait(CSRegSignal_t obj, CSData_t data, uint64_t timeout)
{
    struct CSRegSignal_info_t* _obj = (struct CSRegSignal_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);
    
    struct CSRegSignal_callbackArg_t* arg = &_obj->arg;

    struct CSRegSignal_data_t receive_data = {0};

    struct timespec timeout_spec;
    SATime_timespecAddMs(&timeout_spec, timeout);

    SALOCKER_LOCK(&arg->locker);
    if(SAQueue_count(&arg->queue) == 0)
    {
        SACOND_TIMEWAIT(&arg->cond, &arg->locker, &timeout_spec);
    }
    SAQueue_get(&arg->queue, &receive_data);
    SALOCKER_UNLOCK(&arg->locker);

    CSData_t return_data;
    return_data.type = receive_data.len;
    memcpy(return_data.var.Buff, receive_data.buff, sizeof(uint8_t) * 4);

    return return_data;
}


void CSRegSignal_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args)
{
    struct CSRegSignal_callbackArg_t* buff_info = (struct CSRegSignal_callbackArg_t*)args;
    struct CSRegSignal_data_t receive_data;
    receive_data.len = len;
    memcpy(&receive_data.buff, data, sizeof(uint8_t) * len);

    SALOCKER_LOCK(&buff_info->locker);
    SAQueue_add(&buff_info->queue, &receive_data);
    SACOND_SIGNAL(&buff_info->cond);
    SALOCKER_UNLOCK(&buff_info->locker);
}
