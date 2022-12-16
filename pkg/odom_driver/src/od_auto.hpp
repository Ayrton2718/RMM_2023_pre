#ifndef ODOM_DRIVER_SRC_OD_CONTROLLER_HPP
#define ODOM_DRIVER_SRC_OD_CONTROLLER_HPP

#include <can_smbus/can_smbus.hpp>
#include <math.h>

#include "od_local.hpp"
#include "od_move.hpp"
#include "od_navigation.hpp"

namespace odom_driver
{

typedef struct
{
    Local*          localization;
    MoveParam_t     move_param;
    float           kp_param;
    float           ki_param;
    float           kd_param;
} AutoParam_t;

class Auto
{
private:
    std_api::Timer m_tim;

    Local* m_local;
    Navigation* m_nav;
    Move m_move;

    bool m_arrive_flag;

    float m_kp;
    float m_ki;
    float m_kd;

    Vector_t m_befo_diff;
    Vector_t m_before_place;
    Vector_t m_integral;

    pthread_mutex_t m_locker;
    static void* daemon(void* arg);

public:
    Auto(void);
    Auto(const AutoParam_t* param);

    ~Auto(void);

    void Auto_cons(const AutoParam_t* param);

    void init(void);

    void set_nav(Navigation* nav);

    Vector_t get_pos(void);
    bool is_arrive(void);

    void daemon_start(void);

    void run(void);
};

}

#endif /*ODOM_DRIVER_SRC_OD_CONTROLLER_HPP*/