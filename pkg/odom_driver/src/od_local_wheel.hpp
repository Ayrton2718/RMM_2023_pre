#ifndef ODOM_DRIVER_SRC_OD_LOCAL_WHEEL_HPP
#define ODOM_DRIVER_SRC_OD_LOCAL_WHEEL_HPP

#include <math.h>
#include <can_smbus/can_smbus.hpp>
#include <std_api/std_api.hpp>
#include "od_type.hpp"
#include "od_local.hpp"

namespace odom_driver
{


typedef struct
{
    Vector_t vec_encX;
    Vector_t vec_encY;
    float mpr;  // meter/rad (計測輪の半径)

    struct can_smbus::encoder_param_t encX_param;
    struct can_smbus::encoder_param_t encY_param;
    struct can_smbus::gyro_param_t gyro_param;
} LocalWheelParam_t;


class LocalWheel : public Local
{
private:
    Vector_t m_vec_encX;
    Vector_t m_vec_encY;

    float m_mpr;

    can_smbus::Encoder  m_encX;
    can_smbus::Encoder  m_encY;
    can_smbus::Gyro     m_gyro;

    can_smbus::EncoderPPR_t m_encX_ppr;
    can_smbus::EncoderPPR_t m_encY_ppr;

    float m_sum_x = 0;
    float m_sum_y = 0;
    float m_last_E_x = 0;
    float m_last_E_y = 0;
    float m_last_turn_x = 0;
    float m_last_turn_y = 0;

    Vector_t m_now_estimate;

    pthread_mutex_t m_locker;

    bool m_daemon_running;

    void run(void);
    static void* daemon(void* arg);

public:
    LocalWheel(void);
    LocalWheel(const LocalWheelParam_t* param);

    void LocalWheel_cons(const LocalWheelParam_t* param);

    void init(void);

    void daemon_start(void);

    Vector_t estimate(void);
};

}

#endif /*ODOM_DRIVER_SRC_OD_LOCAL_WHEEL_HPP*/