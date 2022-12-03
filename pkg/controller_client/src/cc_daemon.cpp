#include "cc_daemon.hpp"

#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <can_smbus/can_smbus.hpp>

namespace controller
{

namespace daemon
{

typedef struct
{
    int channel;
    bool is_connected;
    struct sockaddr_in server_addr;
    callback_t callback;

    bool is_safety;

    bool is_update_feedback;
    CCType_feedbackPacket_t feedback;

    void* arg;
} callbackInfo_t;

static callbackInfo_t* searchCallback_nolock(int channel);
extern "C"
{
static void* receiver(void*);
static void* sender(void*);
}

typedef struct
{
    pthread_mutex_t  locker;
    SABuffer_t       callback;

    SATime_timer_t safety_timeout;
} obj_t;

static obj_t g_obj;

void init(void)
{
    SALOCKER_INIT(&g_obj.locker, NULL);
    SABuffer_create(&g_obj.callback, sizeof(callbackInfo_t));

    SATime_timerStart(&g_obj.safety_timeout);

    pthread_t thread;
    SATHREAD_CREATE("controller_receiver", &thread, NULL, receiver, NULL);
    SATHREAD_CREATE("controller_sender", &thread, NULL, sender, NULL);
}


void append(int channel, callback_t callback, void* arg)
{
    callbackInfo_t info;
    info.channel = channel;
    info.is_connected = false;
    info.callback = callback;
    info.arg = arg;

    info.is_update_feedback = true;
    memset(&info.feedback, 0x00, sizeof(CCType_feedbackPacket_t));
    info.feedback.feedback.player_num = channel;

    SALOCKER_LOCK(&g_obj.locker);
    SABuffer_add(&g_obj.callback, &info);
    SALOCKER_UNLOCK(&g_obj.locker);
}


void set_mute(int channel, bool is_enable)
{
    SALOCKER_LOCK(&g_obj.locker);
    {
        callbackInfo_t* info = searchCallback_nolock(channel);
        if(info != NULL)
        {                        
            CCDualsense_feedback_t* feedback = &info->feedback.feedback;
            if(feedback->mute_led != (uint8_t)is_enable)
            {
                info->is_update_feedback = true;
                feedback->mute_led = is_enable;
            }
        }else{
            SALOG_ERROR("controller", "Undefined channle(%d)", channel);
        }
    }   
    SALOCKER_UNLOCK(&g_obj.locker);
}

void set_led(int channel, uint8_t r, uint8_t g, uint8_t b)
{
    SALOCKER_LOCK(&g_obj.locker);
    {
        callbackInfo_t* info = searchCallback_nolock(channel);
        if(info != NULL)
        { 
            CCDualsense_feedback_t* feedback = &info->feedback.feedback;
            if(feedback->lightbar[0] != r || feedback->lightbar[1] != g || feedback->lightbar[2] != b)
            {
                feedback->lightbar[0] = r;
                feedback->lightbar[1] = g;
                feedback->lightbar[2] = b;
            }
        }else{
            SALOG_ERROR("controller", "Undefined channle(%d)", channel);
        }
    }
    SALOCKER_UNLOCK(&g_obj.locker);
}

void set_player(int channel, int num)
{
    SALOCKER_LOCK(&g_obj.locker);
    {
        callbackInfo_t* info = searchCallback_nolock(channel);
        if(info != NULL)
        {
            CCDualsense_feedback_t* feedback = &info->feedback.feedback;
            if(feedback->player_num != num)
            {
                feedback->player_num = num;
            }
        }else{
            SALOG_ERROR("controller", "Undefined channle(%d)", channel);
        }
    }
    SALOCKER_UNLOCK(&g_obj.locker);
}

void set_vibeLarge(int channel, uint8_t value)
{
    SALOCKER_LOCK(&g_obj.locker);
    {
        callbackInfo_t* info = searchCallback_nolock(channel);
        if(info != NULL)
        {
            CCDualsense_feedback_t* feedback = &info->feedback.feedback;
            if(feedback->vibe_large != value)
            {
                feedback->vibe_large = value;
            }
        }else{
            SALOG_ERROR("controller", "Undefined channle(%d)", channel);
        }
    }
    SALOCKER_UNLOCK(&g_obj.locker);
}

void set_vibeSmall(int channel, uint8_t value)
{
    SALOCKER_LOCK(&g_obj.locker);
    {
        callbackInfo_t* info = searchCallback_nolock(channel);
        if(info != NULL)
        {
            CCDualsense_feedback_t* feedback = &info->feedback.feedback;
            if(feedback->vibe_small != value)
            {
                feedback->vibe_small = value;
            }
        }else{
            SALOG_ERROR("controller", "Undefined channle(%d)", channel);
        }
    }
    SALOCKER_UNLOCK(&g_obj.locker);
}


bool is_connected(int channel)
{
    bool is_connected = false;

    SALOCKER_LOCK(&g_obj.locker);
    callbackInfo_t* info = searchCallback_nolock(channel);
    if(info != NULL)
    {
        is_connected = info->is_connected;
    }else{
        SALOG_ERROR("controller", "Undefined channle(%d)", channel);
    }
    SALOCKER_UNLOCK(&g_obj.locker);

    return is_connected;
}


void* receiver(void* arg)
{
    int sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(CCTYPE_SEND_PORT);
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    SASocket_bind(sock, (struct sockaddr*)&bind_addr, sizeof(bind_addr));

    bool emergency_flags[64];

    CCType_sensorPacket_t packet;

    while(1)
    {
        struct sockaddr_in recv_addr;
        int addrlen = sizeof(sockaddr_in);
        ssize_t len = SASocket_recvfrom(sock, &packet, sizeof(CCType_sensorPacket_t), 0, (struct sockaddr*)&recv_addr, (socklen_t*)&addrlen);
        if(0 < len)
        {            
            size_t info_count;

            SALOCKER_LOCK(&g_obj.locker);
            callbackInfo_t* info = searchCallback_nolock(packet.channel);
            if(info != NULL)
            {
                if(info->is_connected == false)
                {
                    info->server_addr.sin_family = AF_INET;
                    info->server_addr.sin_port = htons(CCTYPE_FEEDBACK_PORT);
                    info->server_addr.sin_addr = recv_addr.sin_addr;
                    info->is_connected = true;
                }

                info->is_safety = info->callback(info->arg, (uint8_t*)&packet.sensor, len);
            }
            else
            {
                SALOG_ERROR("controller", "Undefined channle(%d)", packet.channel);
            }
            SATime_timerStart(&g_obj.safety_timeout);
            SALOCKER_UNLOCK(&g_obj.locker);
        }
        else
        {
             SALOG_ERROR("controller", "recv error.");
        }

        SATHREAD_YIELD();
    }
}


void* sender(void* arg)
{
    bool befo_safety = false;
    can_smbus::safetyOff();

    int sock = SASocket_socket(AF_INET, SOCK_DGRAM, 0);

    SATime_timer_t flg_timer;
    SATime_timerStart(&flg_timer);

    while(1)
    {
        bool is_safety = false;
        size_t update_index_count = 0;
        CCType_feedbackPacket_t update_controller_array[16];
        struct sockaddr_in update_sockaddr_array[16];

        SALOCKER_LOCK(&g_obj.locker);
        size_t info_count = SABuffer_count(&g_obj.callback);
        callbackInfo_t* info_array = (callbackInfo_t*)SABuffer_getRaw(&g_obj.callback);
        for(size_t info_i = 0; info_i < info_count; info_i++)
        {
            if(info_array[info_i].is_connected == true)
            {
                is_safety |= info_array[info_i].is_safety;

                if(info_array[info_i].is_update_feedback == true)
                {
                    update_controller_array[update_index_count] = info_array[info_i].feedback;
                    update_sockaddr_array[update_index_count] = info_array[info_i].server_addr;
                    update_index_count++;
                }
            }
        }

        uint64_t safety_timeout = SATime_timerGetMs(&g_obj.safety_timeout);
        SALOCKER_UNLOCK(&g_obj.locker);

        if(is_safety == true || 300 < safety_timeout)
        {
            if(befo_safety == false)
            {
                can_smbus::safetyOn();
                SATime_timerStart(&flg_timer);
                befo_safety = true;
            }

            uint32_t counter = SATime_timerGetMs(&flg_timer) % 500;

            update_index_count = 0;
            CCType_feedbackPacket_t emg_feedback;
            if(counter < 250)
            {
                emg_feedback.feedback.lightbar[0] = 125;
                emg_feedback.feedback.lightbar[1] = 0;
                emg_feedback.feedback.lightbar[2] = 0;
            }else{
                emg_feedback.feedback.lightbar[0] = 255;
                emg_feedback.feedback.lightbar[1] = 255;
                emg_feedback.feedback.lightbar[2] = 255;
            }
            emg_feedback.feedback.mute_led = 1;
            if(SATime_timerGetMs(&flg_timer) < 200)
            {
                emg_feedback.feedback.vibe_large = 255;
            }else{
                emg_feedback.feedback.vibe_large = 0;
            }

            SALOCKER_LOCK(&g_obj.locker);
            size_t info_count = SABuffer_count(&g_obj.callback);
            callbackInfo_t* info_array = (callbackInfo_t*)SABuffer_getRaw(&g_obj.callback);
            for(size_t info_i = 0; info_i < info_count; info_i++)
            {
                if(info_array[info_i].is_connected == true)
                {
                    if(info_array[info_i].is_safety == true)
                    {
                        emg_feedback.feedback.vibe_small = 64;
                    }else{
                        emg_feedback.feedback.vibe_small = 0;
                    }
                    emg_feedback.feedback.player_num = info_array[info_i].channel;
                    update_controller_array[update_index_count] = emg_feedback;
                    update_sockaddr_array[update_index_count] = info_array[info_i].server_addr;
                    update_index_count++;

                    info_array[info_i].is_update_feedback = true;
                }
            }
            SALOCKER_UNLOCK(&g_obj.locker);
        }
        else
        {
            if(befo_safety == true)
            {
                can_smbus::safetyOff();
                befo_safety = false;
            }
        }

        for(size_t update_i = 0; update_i < update_index_count; update_i++)
        {
            SASocket_sendto(sock, &update_controller_array[update_i], sizeof(CCType_feedbackPacket_t), 0, (struct sockaddr*)&update_sockaddr_array[update_i], sizeof(struct sockaddr_in));
        }

        SATime_delayMs(10);
    }
}


static callbackInfo_t* searchCallback_nolock(int channel)
{
    size_t info_count = SABuffer_count(&g_obj.callback);
    callbackInfo_t* info_array = (callbackInfo_t*)SABuffer_getRaw(&g_obj.callback);
    for(size_t i = 0; i < info_count; i++)
    {
        if(info_array[i].channel == channel)
        {
            return &info_array[i];
        }
    }
    return NULL;
}

}

}
