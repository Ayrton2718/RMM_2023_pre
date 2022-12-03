// #include "cc_emergency.hpp"

// #include <netinet/in.h>
// #include <stddef.h>
// #include <stdio.h>
// #include <string.h>
// #include <pthread.h>
// #include "cc_daemon.hpp"

// namespace controller
// {

// Emergency::Emergency()
// {
// }

// Emergency::Emergency(int channel)
// {
//     this->Emergency_cons(channel);
// }

// void Emergency::Emergency_cons(int channel)
// {
//     this->m_channel = channel;
//     this->m_recv_count = 0;
//     SALOCKER_INIT(&this->m_locker, NULL);
// }


// void Emergency::init(void)
// {
//     daemon::append(this->m_channel, callback, this);
// }

// void Emergency::connection_wait(int timeout_ms)
// {
//     SALOG_INFO("controller_server", "connection waiting...");

//     SATime_timer_t tim;
//     SATime_timerStart(&tim);

//     while(1)
//     {
//         SALOCKER_LOCK(&this->m_locker);
//         uint32_t recv_count = this->m_recv_count;
//         SALOCKER_UNLOCK(&this->m_locker);

//         if(recv_count != 0)
//         {
//             SALOG_INFO("controller_server", "connected!");
//             break;
//         }
        
//         if(timeout_ms < SATime_timerGetMs(&tim))
//         {
//             SALOG_ERROR("controller_server", "Connection timeout.");
//             break;
//         }

//         SATHREAD_YIELD();
//     }
// }


// bool Emergency::callback(void* arg, uint8_t* buff, size_t len)
// {
//     bool is_safety = false;
//     Emergency* me = (Emergency*)arg;

//     if(len == sizeof(CCType_emergencyPack_t))
//     {
//         CCType_emergencyPack_t* packet = (CCType_emergencyPack_t*)buff;

//         SALOCKER_LOCK(&me->m_locker);
//         me->m_recv_count++;
//         SALOCKER_LOCK(&me->m_locker);
//         is_safety = (packet->flg == 1)? true : false;
//     }
    
//     return is_safety;
// }

// }