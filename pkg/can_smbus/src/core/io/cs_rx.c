#include "cs_rx.h"

#include "cs_io.h"

#define CSRX_GET_DRIVER_ID(id)   ((uint8_t)(id >> 4))
#define CSRX_GET_DEVICE_ID(id)   ((uint8_t)(id & 0x00F))

#define CSRX_ARG_CHECKER(gw_id, id, reg, callback, args, channel) \
    if(!(gw_id < 4) || CSRX_GET_DRIVER_ID(id) < 0x01 || 0x1F < CSRX_GET_DRIVER_ID(id) || 0x0F < CSRX_GET_DEVICE_ID(id) || !(reg < 16) || !(channel < 2))\
    {\
        SALOG_ERROR("can_smbus", "Receive a illegal id or reg! gateway_id(%d), driver_id(%x), device_id(%x), reg(%x), channel(%d)", gw_id, CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), reg, channel);\
        return;\
    }\


struct CSRx_callbackInfo_t
{
    CSRx_callback_t func;
    void* args;
};

struct CSRx_callbackList_t
{
    size_t count;
    struct CSRx_callbackInfo_t* callbacks;
};

struct CSRx_deviceInfo_t
{
    struct CSRx_callbackList_t regs[2][16];
    struct CSRx_callbackList_t reg_acks[2];
    struct CSRx_callbackList_t req_acks[2];
};

struct CsRx_info_t
{
    pthread_t thread;

    pthread_mutex_t locker;
    
    struct CSRx_deviceInfo_t* devs[4][31][16]; // 0x010 ~ 0x1FF : 0x01~0x1F(30), 0x0~0xF(16)

    struct CSRx_callbackList_t brc_callbacks[4][16];
};


static struct CsRx_info_t g_obj;

static struct CSRx_deviceInfo_t* CSRx_deviceInfoGet(CSType_gatewayId_t gw_id, CSType_canId_t id);
static struct CSRx_deviceInfo_t* CSRx_deviceInfoCreate(CSType_gatewayId_t gw_id, CSType_canId_t id);
static void CSRx_deviceInfoClose(CSType_gatewayId_t gw_id, CSType_canId_t id);

static void CSRx_callbackListAdd(struct CSRx_callbackList_t* list, CSRx_callback_t callback, void* args);
static SABool_t CSRx_callbackListClose(struct CSRx_callbackList_t* list, CSRx_callback_t callback, void* args);
static void CSRx_callbackListCall(struct CSRx_callbackList_t* list, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data);

static void CSRx_regCallback(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, CSType_regChannel_t channel);
static void CSRx_ackCallback(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data);
static void CSRx_brcCallback(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data);
void* CSRx_receiveThread(void* args);


void CSRx_initializer(void)
{
    SALOCKER_INIT(&g_obj.locker, NULL);

    for(size_t gw_i = 0; gw_i < 4; gw_i++)
    {
        memset(&g_obj.devs[gw_i], 0x00, sizeof(struct CSRx_deviceInfo_t*) * 31 * 16);
        memset(&g_obj.brc_callbacks[gw_i], 0x00, sizeof(struct CSRx_deviceInfo_t*) * 16);
    }

    SATHREAD_CREATE("CSRx_receiveThread", &g_obj.thread, NULL, CSRx_receiveThread, NULL);
}


void CSRx_bindReg(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSRx_callback_t callback, void* args, CSType_regChannel_t channel)
{
    CSRX_ARG_CHECKER(gw_id, id, reg, callback, args, channel);

    SALOCKER_LOCK(&g_obj.locker);

    struct CSRx_deviceInfo_t* device_info = CSRx_deviceInfoCreate(gw_id, id);
    CSRx_callbackListAdd(&device_info->regs[channel][reg], callback, args);

    SALOCKER_UNLOCK(&g_obj.locker);
}


void CSRx_closeReg(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, CSRx_callback_t callback, void* args, CSType_regChannel_t channel)
{
    CSRX_ARG_CHECKER(gw_id, id, reg, callback, args, channel);

    SALOCKER_LOCK(&g_obj.locker);

    struct CSRx_deviceInfo_t* device_info = CSRx_deviceInfoGet(gw_id, id);
    if(device_info != NULL)
    {
        if(CSRx_callbackListClose(&device_info->regs[channel][reg], callback, args))
        {
            CSRx_deviceInfoClose(gw_id, id);
        }else{
            SALOG_ERROR("can_smbus", "Receive a reg wasn't registed! gateway_id(%d), driver_id(%x), device_id(%x), channel(%d)", gw_id, CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), channel);
        }
    }else{
        SALOG_ERROR("can_smbus", "Receive a reg wasn't registed! gateway_id(%d), driver_id(%x), device_id(%x), channel(%d)", gw_id, CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), channel);
    }

    SALOCKER_UNLOCK(&g_obj.locker);
}


void CSRx_bindRegAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSRx_callback_t callback, void* args, CSType_regChannel_t channel)
{
    CSRX_ARG_CHECKER(gw_id, id, 0, callback, args, channel);

    SALOCKER_LOCK(&g_obj.locker);

    struct CSRx_deviceInfo_t* device_info = CSRx_deviceInfoCreate(gw_id, id);
    CSRx_callbackListAdd(&device_info->reg_acks[channel], callback, args);

    SALOCKER_UNLOCK(&g_obj.locker);
}

void CSRx_closeRegAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSRx_callback_t callback, void* args, CSType_regChannel_t channel)
{
    CSRX_ARG_CHECKER(gw_id, id, 0, callback, args, channel);

    SALOCKER_LOCK(&g_obj.locker);

    struct CSRx_deviceInfo_t* device_info = CSRx_deviceInfoGet(gw_id, id);
    if(device_info != NULL)
    {
        if(CSRx_callbackListClose(&device_info->reg_acks[channel], callback, args))
        {
            CSRx_deviceInfoClose(gw_id, id);
        }else{
            SALOG_ERROR("can_smbus", "Receive a reg wasn't registed! gateway_id(%d), driver_id(%x), device_id(%x), channel(%d)", gw_id, CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), channel);
        }
    }else{
        SALOG_ERROR("can_smbus", "Receive a reg wasn't registed! gateway_id(%d), driver_id(%x), device_id(%x), channel(%d)", gw_id, CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), channel);
    }

    SALOCKER_UNLOCK(&g_obj.locker);
}

void CSRx_bindReqAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSRx_callback_t callback, void* args, CSType_regChannel_t channel)
{
    CSRX_ARG_CHECKER(gw_id, id, 0, callback, args, channel);

    SALOCKER_LOCK(&g_obj.locker);

    struct CSRx_deviceInfo_t* device_info = CSRx_deviceInfoCreate(gw_id, id);
    CSRx_callbackListAdd(&device_info->req_acks[channel], callback, args);

    SALOCKER_UNLOCK(&g_obj.locker);
}

void CSRx_closeReqAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSRx_callback_t callback, void* args, CSType_regChannel_t channel)
{
    CSRX_ARG_CHECKER(gw_id, id, 0, callback, args, channel);

    SALOCKER_LOCK(&g_obj.locker);

    struct CSRx_deviceInfo_t* device_info = CSRx_deviceInfoGet(gw_id, id);
    if(device_info != NULL)
    {
        if(CSRx_callbackListClose(&device_info->req_acks[channel], callback, args))
        {
            CSRx_deviceInfoClose(gw_id, id);
        }else{
            SALOG_ERROR("can_smbus", "Receive a reg wasn't registed! gateway_id(%d), driver_id(%x), device_id(%x) channel(%d)", gw_id, CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), channel);
        }
    }else{
        SALOG_ERROR("can_smbus", "Receive a reg wasn't registed! gateway_id(%d), driver_id(%x), device_id(%x) channel(%d)", gw_id, CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), channel);
    }

    SALOCKER_UNLOCK(&g_obj.locker);
}


void CSRx_bindBrc(CSType_gatewayId_t gw_id, CSType_canReg_t reg, CSRx_callback_t callback, void* args)
{
    CSRX_ARG_CHECKER(gw_id, 0x201, reg, callback, args, 0);

    SALOCKER_LOCK(&g_obj.locker);
    CSRx_callbackListAdd(&g_obj.brc_callbacks[gw_id][reg], callback, args);
    SALOCKER_UNLOCK(&g_obj.locker);
}

void CSRx_closeBrc(CSType_gatewayId_t gw_id, CSType_canReg_t reg, CSRx_callback_t callback, void* args)
{
    CSRX_ARG_CHECKER(gw_id, 0x201, reg, callback, args, 0);

    SALOCKER_LOCK(&g_obj.locker);
    CSRx_callbackListClose(&g_obj.brc_callbacks[gw_id][reg], callback, args);
    SALOCKER_UNLOCK(&g_obj.locker);
}


static struct CSRx_deviceInfo_t* CSRx_deviceInfoGet(CSType_gatewayId_t gw_id, CSType_canId_t id)
{
    uint8_t driver_index = CSRX_GET_DRIVER_ID(id) - 0x01;
    uint8_t device_index = CSRX_GET_DEVICE_ID(id);

    return g_obj.devs[gw_id][driver_index][device_index];
}

static struct CSRx_deviceInfo_t* CSRx_deviceInfoCreate(CSType_gatewayId_t gw_id, CSType_canId_t id)
{
    uint8_t driver_index = CSRX_GET_DRIVER_ID(id) - 0x01;
    uint8_t device_index = CSRX_GET_DEVICE_ID(id);

    if(g_obj.devs[gw_id][driver_index][device_index] == NULL)
    {
        g_obj.devs[gw_id][driver_index][device_index] = (struct CSRx_deviceInfo_t*)SAMEM_MALLOC(sizeof(struct CSRx_deviceInfo_t));
        memset(g_obj.devs[gw_id][driver_index][device_index], 0x00, sizeof(struct CSRx_deviceInfo_t));
    }

    return g_obj.devs[gw_id][driver_index][device_index];
}

static void CSRx_deviceInfoClose(CSType_gatewayId_t gw_id, CSType_canId_t id)
{
    uint8_t driver_index = CSRX_GET_DRIVER_ID(id) - 0x01;
    uint8_t device_index = CSRX_GET_DEVICE_ID(id);

    SABool_t all_unusing = SABOOL_TRUE;
    struct CSRx_deviceInfo_t* my_deviceReg = g_obj.devs[gw_id][driver_index][device_index];
    for(size_t type_i = 0; type_i < 2; type_i++)
    {
        for(size_t reg_i = 0; reg_i < 16; reg_i++)
        {
            if(my_deviceReg->regs[type_i][reg_i].count != 0)
            {
                all_unusing = SABOOL_FALSE;
            }
        }

        if(0 < my_deviceReg->reg_acks[type_i].count)
        {
            all_unusing = SABOOL_FALSE;
        }

        if(0 < my_deviceReg->req_acks[type_i].count)
        {
            all_unusing = SABOOL_FALSE;
        }
    }

    if(all_unusing)
    {
        SAMEM_FREE(g_obj.devs[gw_id][driver_index][device_index]);
        g_obj.devs[gw_id][driver_index][device_index] = NULL;
    }
}


static void CSRx_callbackListAdd(struct CSRx_callbackList_t* list, CSRx_callback_t callback, void* args)
{
    if(list->count == 0)
    {
        list->callbacks = (struct CSRx_callbackInfo_t*)SAMEM_MALLOC(sizeof(struct CSRx_callbackInfo_t) * 1);
    }else{
        list->callbacks = (struct CSRx_callbackInfo_t*)SAMEM_REALLOC(list->callbacks, sizeof(struct CSRx_callbackInfo_t) * (list->count + 1));
    }

    list->callbacks[list->count].func = callback;
    list->callbacks[list->count].args = args;
    list->count++;
}

static SABool_t CSRx_callbackListClose(struct CSRx_callbackList_t* list, CSRx_callback_t callback, void* args)
{
    SABool_t is_hit = SABOOL_FALSE;
    for(size_t callback_i = 0; callback_i < list->count; callback_i++)
    {
        if((list->callbacks[callback_i].func == callback) && (list->callbacks[callback_i].args == args))
        {
            for(size_t push_i = callback_i + 1; push_i < list->count; push_i++)
            {
                list->callbacks[push_i - 1].func = list->callbacks[push_i].func;
                list->callbacks[push_i - 1].args = list->callbacks[push_i].args;
            }
            list->count--;

            if(list->count == 0)
            {
                SAMEM_FREE(list->callbacks);
                list->callbacks = NULL;
            }

            is_hit = SABOOL_TRUE;
        }
    }
    return is_hit;
}

static void CSRx_callbackListCall(struct CSRx_callbackList_t* list, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data)
{
    for(size_t callback_i = 0; callback_i < list->count; callback_i++)
    {
        list->callbacks[callback_i].func(CSTYPE_GET_NUMBER_ID(id), CSTYPE_GET_NUMBER_REG(reg), len, data, list->callbacks[callback_i].args);
    }

    if(list->count == 0)
    {
        SALOG_ERROR("can_smbus", "Receive a reg wasn't registed! driver_id(%x), device_id(%x) channel(%d)", CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), 0);
    }
}


static void CSRx_regCallback(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data, CSType_regChannel_t channel)
{
    CSRX_ARG_CHECKER(gw_id, id, reg, NULL, NULL, 0);

    SALOCKER_LOCK(&g_obj.locker);
    struct CSRx_deviceInfo_t* device_info = CSRx_deviceInfoGet(gw_id, id);
    if(device_info != NULL)
    {
        CSRx_callbackListCall(&device_info->regs[channel][reg], id, reg, len, data);
    }else{
        SALOG_ERROR("can_smbus", "Receive a reg wasn't registed! gateway_id(%d), driver_id(%x), device_id(%x) channel(%d)", gw_id, CSRX_GET_DRIVER_ID(id), CSRX_GET_DEVICE_ID(id), 0);
    }

    SALOCKER_UNLOCK(&g_obj.locker);
}


static void CSRx_ackCallback(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data)
{
    CSRX_ARG_CHECKER(gw_id, id, reg, NULL, NULL, 0);

    if(1 < len)
    {
        SALOCKER_LOCK(&g_obj.locker);
        struct CSRx_deviceInfo_t* device_info = CSRx_deviceInfoGet(gw_id, id);
        if(device_info != NULL)
        {
            if(CSTYPE_IS_WRITEACK_REG(data[0]))
            {
                if(CSTYPE_IS_SYS_REG(data[0]))
                {
                    CSRx_callbackListCall(&device_info->reg_acks[CSType_regChannel_SYSTEM], id, reg, len, data);
                }else{
                    CSRx_callbackListCall(&device_info->reg_acks[CSType_regChannel_USER], id, reg, len, data);
                }
            }
            else if(CSTYPE_IS_REQUESTACK_REG(data[0]))
            {
                if(CSTYPE_IS_SYS_REG(data[0]))
                {
                    CSRx_callbackListCall(&device_info->req_acks[CSType_regChannel_SYSTEM], id, reg, len, data);
                }else{
                    CSRx_callbackListCall(&device_info->req_acks[CSType_regChannel_USER], id, reg, len, data);
                }
            }
        }
        
        SALOCKER_UNLOCK(&g_obj.locker);
    }
}


static void CSRx_brcCallback(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* data)
{
    CSRX_ARG_CHECKER(gw_id, id, reg, NULL, NULL, 0);

    SALOCKER_LOCK(&g_obj.locker);
    CSRx_callbackListCall(&g_obj.brc_callbacks[gw_id][reg], id, reg, len, data);
    SALOCKER_UNLOCK(&g_obj.locker);
}


void* CSRx_receiveThread(void* args)
{
    struct CSIo_dataPack_t data_pack;

    while(1)
    {
        if(CSIo_read(&data_pack))
        {
            CSType_gatewayId_t gw_id = data_pack.gw_id;
            for(size_t i = 0; i < data_pack.data_count; i++)
            {
                CSType_canId_t id = data_pack.data[i].id;
                CSType_canReg_t reg = data_pack.data[i].reg;
                size_t len = data_pack.data[i].len;
                const uint8_t* data =  data_pack.data[i].data;

#ifdef CS_DEBUG_ON
                SALOG_INFO("can_smbus", "Recv from 0x%03x, 0x%02x, %d [0x%02x, 0x%02x, 0x%02x, 0x%02x]", id, reg, len, data[0], data[1], data[2], data[3]);
#endif /*CS_DEBUG_ON*/

                if(CSTYPE_IS_DRIV_ID(id))
                {
                    if(CSTYPE_IS_WRITE_REG(reg))
                    {
                        if(CSTYPE_IS_SYS_REG(reg))
                        {
                            CSRx_regCallback(gw_id, CSTYPE_GET_NUMBER_ID(id), CSTYPE_GET_NUMBER_REG(reg), len, data, CSType_regChannel_SYSTEM);   
                        }else{
                            CSRx_regCallback(gw_id, CSTYPE_GET_NUMBER_ID(id), CSTYPE_GET_NUMBER_REG(reg), len, data, CSType_regChannel_USER);
                        }
                    }
                    else if(CSTYPE_IS_ACK_REG(reg))
                    {
                        CSRx_ackCallback(data_pack.gw_id, CSTYPE_GET_NUMBER_ID(id), CSTYPE_GET_NUMBER_REG(reg), len, data);
                    }
                    else if(CSTYPE_IS_BRC_REG(reg))
                    {
                        CSRx_brcCallback(data_pack.gw_id, CSTYPE_GET_NUMBER_ID(id), CSTYPE_GET_NUMBER_REG(reg), len, data);
                    }
                    else
                    {
                        SALOG_WARN("can_smbus", "Pick up a invited packet. gateway_id(%d), id(%x), reg(%x)", gw_id, id, reg);
                    }
                }
                else
                {
                    SALOG_WARN("can_smbus", "Pick up a invited packet. gateway_id(%d), id(%x), reg(%x)", gw_id, id, reg);
                }
            }
        }
    }

    return NULL;
}
