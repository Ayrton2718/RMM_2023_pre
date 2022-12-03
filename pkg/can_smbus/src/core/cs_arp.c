#include "cs_arp.h"

#include "io/cs_rx.h"
#include "io/cs_tx.h"
#include "cs_checker.h"

struct CSArp_callbackArg_t
{
    pthread_mutex_t locker;

    SABuffer_t list;
};

struct CSArp_info_t
{
    CSChecker_t checker;
    CSType_gatewayId_t gw_id;

    struct CSArp_callbackArg_t arg;
};


static void CSArp_execute(CSArp_t obj);
void CSArp_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args);


CSArp_t CSArp_create(CSType_gatewayId_t gw_id)
{
    struct CSArp_info_t* _obj = (struct CSArp_info_t*)SAMEM_MALLOC(sizeof(struct CSArp_info_t));
    memset(_obj, 0x00, sizeof(struct CSArp_info_t));

    _obj->checker = CSCHECKER_ID;

    _obj->gw_id = gw_id;

    SALOCKER_INIT(&_obj->arg.locker, NULL);
    SABuffer_create(&_obj->arg.list, sizeof(struct CSArp_data_t));

    CSRx_bindBrc(gw_id, 0, CSArp_callback, &_obj->arg);
    return _obj;
}


void CSArp_close(CSArp_t obj)
{
    if(obj != NULL)
    {
        struct CSArp_info_t* _obj = (struct CSArp_info_t*)obj;
        CSCHECKER_CHECK(obj, _obj->checker);

        CSRx_closeBrc(_obj->gw_id, 0, CSArp_callback, &_obj->arg);
        SALOCKER_LOCK(&_obj->arg.locker);
        SABuffer_destructor(&_obj->arg.list);
        SALOCKER_UNLOCK(&_obj->arg.locker);

        SALOCKER_DESTROY(&_obj->arg.locker);
        SAMEM_FREE(obj);
    }
}

void CSArp_printf(CSArp_t obj)
{
    struct CSArp_info_t* _obj = (struct CSArp_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    CSArp_execute(obj);
    
    SALOCKER_LOCK(&_obj->arg.locker);
    struct CSArp_data_t* set_data = (struct CSArp_data_t*)SABuffer_getRaw(&_obj->arg.list);
    for(size_t i = 0; i < SABuffer_count(&_obj->arg.list); i++)
    {
        if(set_data[i].is_started)
        {
            SALOG_INFO("can_smbus", "%s(%x) is online. is_start(*)", CSType_driverConvertName(set_data[i].id >> 4), set_data[i].id & 0x0F);
        }else{
            SALOG_INFO("can_smbus", "%s(%x) is online. is_start( )", CSType_driverConvertName(set_data[i].id >> 4), set_data[i].id & 0x0F);
        }
    }
    SALOCKER_UNLOCK(&_obj->arg.locker);
}

void CSArp_getList(CSArp_t obj, struct CSArp_data_t* list, size_t* list_size)
{
    struct CSArp_info_t* _obj = (struct CSArp_info_t*)obj;
    CSCHECKER_CHECK(obj, _obj->checker);

    CSArp_execute(obj);
    
    SALOCKER_LOCK(&_obj->arg.locker);
    if(*list_size < SABuffer_count(&_obj->arg.list))
    {
        SALOG_ERROR("can_smbus", "Overflowed list_size.");
        *list_size = SABuffer_count(&_obj->arg.list);
    }

    memcpy(list, SABuffer_getRaw(&_obj->arg.list), sizeof(struct CSArp_data_t) * (*list_size));
    SALOCKER_UNLOCK(&_obj->arg.locker);
}

static void CSArp_execute(CSArp_t obj)
{
    struct CSArp_info_t* _obj = (struct CSArp_info_t*)obj;

    SALOCKER_LOCK(&_obj->arg.locker);
    SABuffer_removeAll(&_obj->arg.list);
    SALOCKER_UNLOCK(&_obj->arg.locker);

    uint8_t buff[4];
    CSTx_sendBrc(_obj->gw_id, CSType_brcReg_Arp, 0, buff, CSTx_priority_High);
    SATime_delayMs(800);
    CSTx_sendBrc(_obj->gw_id, CSType_brcReg_Arp, 0, buff, CSTx_priority_High);
    SATime_delayMs(800);
    CSTx_sendBrc(_obj->gw_id, CSType_brcReg_Arp, 0, buff, CSTx_priority_High);
    SATime_delayMs(800);
    CSTx_sendBrc(_obj->gw_id, CSType_brcReg_Arp, 0, buff, CSTx_priority_High);
    SATime_delayMs(800);
}

void CSArp_callback(CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, void* args)
{
    struct CSArp_callbackArg_t* _arg = (struct CSArp_callbackArg_t*)args;
    
    struct CSArp_data_t set_data;
    set_data.id = id;
    if((data[0] & 0x01) == 1)
    {
        set_data.is_started = SABOOL_TRUE;
    }else{
        set_data.is_started = SABOOL_FALSE;
    }

    SALOCKER_LOCK(&_arg->locker);
    SABuffer_add(&_arg->list, &set_data);
    SALOCKER_UNLOCK(&_arg->locker);
}
