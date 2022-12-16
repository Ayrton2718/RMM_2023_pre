#ifndef CAN_SMBUS_SRC_SERVO_HPP
#define CAN_SMBUS_SRC_SERVO_HPP

#include "core/cs_manager.h"
#include "core/cs_send.h"
#include "core/cs_send_ack.h"
#include "core/cs_reg_share.h"
#include "core/cs_reg_signal.h"

#include "type.hpp"

namespace can_smbus
{

struct servo_param_t
{
    int id;
    Port_t port;
    Gateway_t gw_id;
};

class Servo
{
private:
    CSManager_t m_manager;

    CSSend_t r_angle;
    CSSendAck_t r_enable;

public:
    Servo(void);
    Servo(int id, Port_t port, Gateway_t gw_id = Gateway_1);
    Servo(const struct servo_param_t* param);

    void Servo_cons(int id, Port_t port, Gateway_t gw_id = Gateway_1);
    void Servo_cons(const struct servo_param_t *param);

    void init(void);

    void enable(void);
    void disable(void);

    void set_angle(float angle);
};

}

#endif /*CAN_SMBUS_SRC_SERVO_HPP*/