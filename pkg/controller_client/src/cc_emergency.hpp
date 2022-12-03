// #ifndef CONTROLLER_CLIENT_CC_EMERGENCY_HPP
// #define CONTROLLER_CLIENT_CC_EMERGENCY_HPP

// #include "cc_type.h"

// namespace controller
// {

// class Emergency
// {
// private:
//     int                 m_channel;

//     uint32_t            m_recv_count;

//     pthread_mutex_t     m_locker;

//     static bool callback(void* arg, uint8_t* buff, size_t len);

// public:
//     Emergency();
//     Emergency(int channel);

//     void Emergency_cons(int channel);

//     void init(void);

//     void connection_wait(int timeout_ms = 500000);
// };

// }

// #endif /*CONTROLLER_CLIENT_CC_EMERGENCY_HPP*/