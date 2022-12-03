#include "cc_controller.hpp"

#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "cc_daemon.hpp"

namespace controller
{

Controller::Controller()
{
}

Controller::Controller(int channel, EmergencyBtn_t eb)
{
    this->Controller_cons(channel, eb);
}

void Controller::Controller_cons(int channel, EmergencyBtn_t eb)
{
    this->m_channel = channel;
    
    this->m_eb = eb;
    this->m_safety_count = 0;
    this->m_safety_changed = false;
    this->m_safety_flg = false;

    SALOCKER_INIT(&this->m_locker, NULL);
    
    this->m_packet_flg = 0;
    memset(this->m_packet, 0x00, sizeof(CCDualsense_sensor_t) * 2);
}


void Controller::init(void)
{
    daemon::append(this->m_channel, callback, this);
}

void Controller::connection_wait(int timeout_ms)
{
    SALOG_INFO("controller_server", "connection waiting(%d)...", this->m_channel);

    SATime_timer_t tim;
    SATime_timerStart(&tim);

    while(1)
    {
        if(daemon::is_connected(this->m_channel) == true)
        {
            SALOG_INFO("controller_server", "connected!");
            break;
        }

        if(timeout_ms < SATime_timerGetMs(&tim))
        {
            SALOG_ERROR("controller_server", "Connection timeout.");
            break;
        }

        SATime_delayMs(10);
    }
}


CCDualsense_sensor_t Controller::get_sensor(void)
{
    return this->m_packet[this->m_packet_flg % 2];
}


void Controller::set_mute(bool is_enable)
{
    daemon::set_mute(this->m_channel, is_enable);
}

void Controller::set_led(uint8_t r, uint8_t g, uint8_t b)
{
    daemon::set_led(this->m_channel, r, g, b);
}

void Controller::set_player(int num)
{
    daemon::set_player(this->m_channel, num);
}

void Controller::set_vibeLarge(float rate)
{
    if(rate < 0)
    {
        rate = 0;
    }else if(1 < rate){
        rate = 1;
    }
    daemon::set_vibeLarge(this->m_channel, (uint8_t)(rate * 255));
}

void Controller::set_vibeSmall(float rate)
{
    if(rate < 0)
    {
        rate = 0;
    }else if(1 < rate){
        rate = 1;
    }
    daemon::set_vibeSmall(this->m_channel, (uint8_t)(rate * 255));
}


bool Controller::callback(void* arg, uint8_t* buff, size_t len)
{
    Controller* me = (Controller*)arg;
    if(len == sizeof(CCType_sensorPacket_t))
    {
        EmergencyBtn_t btn;
        CCDualsense_sensor_t* sensor = (CCDualsense_sensor_t*)buff;

        SALOCKER_LOCK(&me->m_locker);
        btn = me->m_eb;
        SALOCKER_UNLOCK(&me->m_locker);

        memcpy(&me->m_packet[(me->m_packet_flg + 1) % 2], sensor, sizeof(CCDualsense_sensor_t));
        me->m_packet_flg += 1;

        bool is_safety;
        switch (btn)
        {
        case EmergencyBtn_Ps:
            is_safety = (sensor->btn3.PS == 1)? true : false;
            break;
        
        case EmergencyBtn_Mute:
            is_safety = (sensor->btn3.Mute == 1)? true : false;
            break;

        case EmergencyBtn_TouchPad:
            is_safety = (sensor->btn3.Touch == 1)? true : false;
            break;
        
        default:
            is_safety = false;
            break;
        }

        if(is_safety)
        {
            me->m_safety_count++;
        }else{
            me->m_safety_count = 0;
            me->m_safety_changed = false;
        }

        if(me->m_safety_changed == false && 15 < me->m_safety_count)
        {
            me->m_safety_flg = !me->m_safety_flg;
            me->m_safety_changed = true;
        }
    }
    
    return me->m_safety_flg;
}

}