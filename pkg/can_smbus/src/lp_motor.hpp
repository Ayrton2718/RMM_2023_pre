#ifndef CAN_SMBUS_SRC_LP_MOTOR_HPP
#define CAN_SMBUS_SRC_LP_MOTOR_HPP

#include "core/cs_manager.h"
#include "core/cs_send.h"
#include "core/cs_send_ack.h"
#include "core/cs_reg_share.h"
#include "core/cs_reg_signal.h"

#include "type.hpp"

namespace can_smbus
{

struct lp_motor_param_t
{
    int id;
    Port_t port;
    DirectRota_t direction;
    Gateway_t gw_id;
};

class LPMotor
{
private:
    CSManager_t m_manager;

    CSSend_t r_power;
    CSSendAck_t r_acc;

    int m_direction_num;

public:
    LPMotor(void);
    LPMotor(int id, Port_t port, DirectRota_t direction = DirectRota_FORWARD, Gateway_t gw_id = Gateway_1);
    LPMotor(const struct lp_motor_param_t* param);

    void LPMotor_cons(int id, Port_t port, DirectRota_t direction = DirectRota_FORWARD, Gateway_t gw_id = Gateway_1);
    void LPMotor_cons(const struct lp_motor_param_t* param);

    void init(void);

    void set_acc(float acc); // 0.1 ~ 1がいいけど、1以上だと加速度がない状態になる。

    void set_power(float power);
};

}

#endif /*CAN_SMBUS_SRC_LP_MOTOR_HPP*/