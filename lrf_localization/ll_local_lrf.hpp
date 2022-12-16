#ifndef LRF_LOCALIZATION_SRC_LL_LOCAL_LRF_HPP
#define LRF_LOCALIZATION_SRC_LL_LOCAL_LRF_HPP

#include <odom_driver/odom_driver.hpp>

#include "ll_map.h"
#include "ll_g2.h"
#include "ll_history.h"
#include "ll_pf.h"
#include "ll_scan_type.h"
#include "ll_timer.h"
#include "ll_tomas.h"

namespace lrf_localization
{

typedef struct
{
    float init_x;
    float init_y;
    float init_teta;

    odom_driver::LocalWheelParam_t wheel_param;
} LocalLrfParam_t;


class LocalLrf : public odom_driver::Local
{
private:    
#ifndef LL_LRF_SIM
    LLG2_t      m_g2;
#endif /*LL_LRF_SIM*/

    LLTimer_t   m_timer;
    LLTomas_t   m_tomas;
    LLHistory_t m_history;
    LLPf_t      m_pf;

#ifndef LL_LRF_SIM
    odom_driver::Vector_t   m_befo_wheel;
    odom_driver::LocalWheel m_local;
#else
    LLTomas_t   m_virtual_tomas;
    // LLMap_t     m_map;
#endif /*LL_LRF_SIM*/

    pthread_mutex_t         m_locker;
    odom_driver::Vector_t   m_now_estimate;

    bool m_daemon_running;

    void run(void);
    static void* daemon(void* arg);

    void wheel_run(void);
    static void* daemon_wheel(void* arg);

public:
    LocalLrf(void);
    LocalLrf(const LocalLrfParam_t* param);

    void LocalLrf_cons(const LocalLrfParam_t* param);

    void init(void);

    void daemon_start(void);

    odom_driver::Vector_t estimate(void);
};

};

#endif /*LRF_LOCALIZATION_SRC_LL_LOCAL_LRF_HPP*/