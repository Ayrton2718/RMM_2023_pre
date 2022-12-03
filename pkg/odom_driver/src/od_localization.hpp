#ifndef ODOM_DRIVER_SRC_OD_LOCALIZATION_HPP
#define ODOM_DRIVER_SRC_OD_LOCALIZATION_HPP

#include <can_smbus/can_smbus.hpp>
#include <std_api/std_api.hpp>
#include "od_type.hpp"

namespace odom_driver
{

typedef struct
{
    Vector_t vec_encX;
    Vector_t vec_encY;
    float mpr;  // meter per rad

    struct can_smbus::encoder_param_t encX_param;
    struct can_smbus::encoder_param_t encY_param;
    struct can_smbus::gyro_param_t gyro_param;
} LocalizationParam_t;



class Localization
{
private:
    Vector_t m_vec_encX;
    Vector_t m_vec_encY;

    float m_mpr;

    can_smbus::Encoder m_encX;
    can_smbus::Encoder m_encY;
    can_smbus::Gyro   m_gyro;

    std_api::Timer m_timer;

public:
    Localization(void);
    Localization(const LocalizationParam_t* param);

    void Localization_cons(const LocalizationParam_t* param);

    void init(void);

    Vector_t estimate(void);
};

}

#endif /*ODOM_DRIVER_SRC_OD_LOCALIZATION_HPP*/