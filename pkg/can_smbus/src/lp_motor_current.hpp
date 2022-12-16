#ifndef CAN_SMBUS_SRC_LP_MOTOR_CURRENT_HPP
#define CAN_SMBUS_SRC_LP_MOTOR_CURRENT_HPP

#include "core/cs_manager.h"
#include "core/cs_send.h"
#include "core/cs_send_ack.h"
#include "core/cs_reg_share.h"
#include "core/cs_reg_signal.h"

#include "type.hpp"

namespace can_smbus
{

typedef enum
{
    LPMotorCurrentMode_Voltage = 0,
    LPMotorCurrentMode_Current
} LPMotorCurrentMode_t;

struct lp_motor_current_param_t
{
    int id;
    Port_t port;
    LPMotorCurrentMode_t mode;
    DirectRota_t direction;
    Gateway_t gw_id;
};


class LPMotorCurrent
{
private:
    CSManager_t m_manager;

    CSSend_t r_kp;
    CSSend_t r_ki;
    CSSend_t r_kd;

    CSSend_t r_power;
    CSSendAck_t r_acc;
    CSSend_t r_currenterror;

    CSSendAck_t r_mode;
    CSSend_t r_current;

    LPMotorCurrentMode_t m_mode;

    CSRegShare_t r_var;

    int m_direction_num;
public:
    LPMotorCurrent(void);
    LPMotorCurrent(int id, Port_t port, LPMotorCurrentMode_t mode, DirectRota_t direction = DirectRota_FORWARD, Gateway_t gw_id = Gateway_1);
    LPMotorCurrent(const struct lp_motor_current_param_t* param);

    void LPMotorCurrent_cons(int id, Port_t port, LPMotorCurrentMode_t mode, DirectRota_t direction = DirectRota_FORWARD, Gateway_t gw_id = Gateway_1);
    void LPMotorCurrent_cons(const struct lp_motor_current_param_t* param);

    void init(void);

    void set_acc(float acc); // 0.1 ~ 1がいいけど、1以上だと加速度がない状態になる。
    void set_pid(float kp, float ki, float kd);
    void set_currenterror(float currenterror);

    void set_power(float power);
    void set_current(float current);

    float getCurrent(void);
};

}

#endif /*CAN_SMBUS_SRC_LP_MOTOR_CURRENT_HPP*/