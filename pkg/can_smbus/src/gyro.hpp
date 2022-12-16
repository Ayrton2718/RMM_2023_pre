#ifndef CAN_SMBUS_SRC_GYRO_HPP
#define CAN_SMBUS_SRC_GYRO_HPP

#include "core/cs_manager.h"
#include "core/cs_send.h"
#include "core/cs_reg_share.h"
#include "core/cs_reg_signal.h"

#include "type.hpp"

namespace can_smbus
{

typedef enum
{
    GyroMode_AccX = 0,
    GyroMode_AccY,
    GyroMode_AccZ,

    GyroMode_GyroX,
    GyroMode_GyroY,
    GyroMode_GyroZ,

    GyroMode_Roll,
    GyroMode_Pitch,
    GyroMode_Yaw,

    GyroMode_Quaternion0,
    GyroMode_Quaternion1,
    GyroMode_Quaternion2,
    GyroMode_Quaternion3,
} GyroMode_t;


struct gyro_param_t
{
    int id;
    GyroMode_t mode;
    Interval_t interval;
    Gateway_t gw_id;
};


class Gyro
{
private:
    CSManager_t m_manager;

    Interval_t m_interval;

    CSRegShare_t r_var;

public:
    Gyro(void);
    Gyro(int id, GyroMode_t mode, Interval_t interval = Interval_20Hz, Gateway_t gw_id = Gateway_1);
    Gyro(const struct gyro_param_t* param);

    void Gyro_cons(int id, GyroMode_t mode, Interval_t interval = Interval_20Hz, Gateway_t gw_id = Gateway_1);
    void Gyro_cons(const struct gyro_param_t *param);

    void init(void);

    float get(void);
};

}

#endif /*CAN_SMBUS_SRC_GYRO_HPP*/