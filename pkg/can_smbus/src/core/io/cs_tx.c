#include "cs_tx.h"

#include "cs_io.h"


struct CsTx_info_t
{
    pthread_t thread[4];

    pthread_mutex_t locker[4];
    pthread_cond_t cond[4];
    
    SAQueue_t queue[4][3];
    struct CSType_csFrame_t buffer[4][3][32];
};


static struct CsTx_info_t g_obj;


static void CSTx_sendFrame(CSType_gatewayId_t gw_id, const struct CSType_csFrame_t* frame, CSTx_priority_t priority);
void* CSTx_sendThread(void* args);


void CSTx_initializer(void)
{
    for(CSType_gatewayId_t gw_i = 0; gw_i < 4; gw_i++)
    {
        SALOCKER_INIT(&g_obj.locker[gw_i], NULL);
        SACOND_INIT(&g_obj.cond[gw_i], NULL);

        SAQueue_init(&g_obj.queue[gw_i][0], sizeof(struct CSType_csFrame_t), 32, g_obj.buffer[gw_i][0]);
        SAQueue_init(&g_obj.queue[gw_i][1], sizeof(struct CSType_csFrame_t), 32, g_obj.buffer[gw_i][1]);
        SAQueue_init(&g_obj.queue[gw_i][2], sizeof(struct CSType_csFrame_t), 32, g_obj.buffer[gw_i][2]);

        char thread_name[64];
        sprintf(thread_name, "CSTx_sendThread_%d", gw_i);
        SATHREAD_CREATE(thread_name, &g_obj.thread[gw_i], NULL, CSTx_sendThread, (void*)gw_i);
    }
}


void CSTx_sendBrc(CSType_gatewayId_t gw_id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority)
{
    struct CSType_csFrame_t frame;
    frame.id = CSTYPE_BROADCAST_ADDR;
    frame.reg = reg;
    frame.len = len;
    memcpy(frame.data, buff, sizeof(uint8_t) * 4);

    CSTx_sendFrame(gw_id, &frame, priority);
}


void CSTx_sendReg(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority, CSType_regChannel_t channel)
{
    struct CSType_csFrame_t frame;
    frame.id = CSTYPE_GET_HOST_ID(id);
    frame.reg = CSTYPE_GET_WRITE_REG(reg);
    if(channel == CSType_regChannel_SYSTEM)
    {
        frame.reg = CSTYPE_GET_SYS_REG(frame.reg);
    }else if(channel == CSType_regChannel_USER)
    {
        frame.reg = CSTYPE_GET_USER_REG(frame.reg);
    }
    frame.len = len;
    memcpy(frame.data, buff, sizeof(uint8_t) * 4);

    CSTx_sendFrame(gw_id, &frame, priority);
}


void CSTx_sendRegAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority, CSType_regChannel_t channel)
{
    struct CSType_csFrame_t frame;
    frame.id = CSTYPE_GET_HOST_ID(id);
    frame.reg = CSTYPE_GET_WRITEACK_REG(reg);
    if(channel == CSType_regChannel_SYSTEM)
    {
        frame.reg = CSTYPE_GET_SYS_REG(frame.reg);
    }else if(channel == CSType_regChannel_USER)
    {
        frame.reg = CSTYPE_GET_USER_REG(frame.reg);
    }
    frame.len = len;
    memcpy(frame.data, buff, sizeof(uint8_t) * 4);

    CSTx_sendFrame(gw_id, &frame, priority);
}

void CSTx_sendReq(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority, CSType_regChannel_t channel)
{
    struct CSType_csFrame_t frame;
    frame.id = CSTYPE_GET_HOST_ID(id);
    frame.reg = CSTYPE_GET_REQUEST_REG(reg);
    if(channel == CSType_regChannel_SYSTEM)
    {
        frame.reg = CSTYPE_GET_SYS_REG(frame.reg);
    }else if(channel == CSType_regChannel_USER)
    {
        frame.reg = CSTYPE_GET_USER_REG(frame.reg);
    }
    frame.len = len;
    memcpy(frame.data, buff, sizeof(uint8_t) * 4);    

    CSTx_sendFrame(gw_id, &frame, priority);
}

void CSTx_sendReqAck(CSType_gatewayId_t gw_id, CSType_canId_t id, CSType_canReg_t reg, uint8_t len, const uint8_t* buff, CSTx_priority_t priority, CSType_regChannel_t channel)
{
    struct CSType_csFrame_t frame;
    frame.id = CSTYPE_GET_HOST_ID(id);
    frame.reg = CSTYPE_GET_REQUESTACK_REG(reg);
    if(channel == CSType_regChannel_SYSTEM)
    {
        frame.reg = CSTYPE_GET_SYS_REG(frame.reg);
    }else if(channel == CSType_regChannel_USER)
    {
        frame.reg = CSTYPE_GET_USER_REG(frame.reg);
    }
    frame.len = len;
    memcpy(frame.data, buff, sizeof(uint8_t) * 4);    

    CSTx_sendFrame(gw_id, &frame, priority);
}


static void CSTx_sendFrame(CSType_gatewayId_t gw_id, const struct CSType_csFrame_t* frame, CSTx_priority_t priority)
{
    SALOCKER_LOCK(&g_obj.locker[gw_id]);
    SAQueue_add(&g_obj.queue[gw_id][priority], frame);
    SALOCKER_UNLOCK(&g_obj.locker[gw_id]);

    SACOND_SIGNAL(&g_obj.cond[gw_id]);
}


void* CSTx_sendThread(void* args)
{
    CSType_gatewayId_t gw_id = (CSType_gatewayId_t)args;
    
    SAQueue_t skip_queue;
    struct CSType_csFrame_t skip_queue_buffer[16];

    SAQueue_init(&skip_queue, sizeof(struct CSType_csFrame_t), 16, skip_queue_buffer);

    struct CSIo_dataPack_t send_pack;
    send_pack.gw_id = gw_id;
    
    pthread_mutex_t* locker = &g_obj.locker[gw_id];
    pthread_cond_t* cond = &g_obj.cond[gw_id];
    SAQueue_t* queue = g_obj.queue[gw_id];
    
    while(1)
    {
        size_t all_count = 0;
        SALOCKER_LOCK(locker);
        all_count = SAQueue_count(&queue[0]) +  SAQueue_count(&queue[1]) +  SAQueue_count(&queue[2]);
        if(all_count == 0)
        {
            SACOND_WAIT(cond, locker);
            all_count = SAQueue_count(&queue[0]) +  SAQueue_count(&queue[1]) +  SAQueue_count(&queue[2]);
        }
        SALOCKER_UNLOCK(locker);

        size_t send_counter = 0;
        for(size_t priority = 2; 0 < priority; priority--)
        {
            while((0 < SAQueue_count(&queue[priority])) && (send_counter < 4))
            {                
                struct CSType_csFrame_t tmp_frame;
                SAQueue_get(&queue[priority], &tmp_frame);

                SABool_t is_effective = SABOOL_TRUE;
                for(size_t i = 0; i < send_counter; i++)
                {
                    if(tmp_frame.id == send_pack.data[i].id)
                    {
                        SAQueue_add(&skip_queue, &tmp_frame);
                        is_effective = SABOOL_FALSE;
                        break;
                    }
                }

                if(is_effective)
                {
#ifdef CS_DEBUG_ON
                    SALOG_INFO("can_smbus", "Send to 0x%03x, 0x%02x, %d [0x%02x, 0x%02x, 0x%02x, 0x%02x]", tmp_frame.id, tmp_frame.reg, tmp_frame.len, tmp_frame.data[0], tmp_frame.data[1], tmp_frame.data[2], tmp_frame.data[3]);
#endif /*CS_DEBUG_ON*/
                    memcpy(&send_pack.data[send_counter], &tmp_frame, sizeof(struct CSType_csFrame_t));
                    send_counter++;
                }
            }
            
            while(0 < SAQueue_count(&skip_queue))
            {
                SAQueue_addFront(&queue[priority], SAQueue_getRaw(&skip_queue));
            }

            SAQueue_clear(&skip_queue);
        }

        send_pack.data_count = send_counter;
        CSIo_send(&send_pack);

        SATime_delayMs(1);
    }

    return NULL;
}
