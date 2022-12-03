#ifndef CAN_SMBUS_SRC_ABS_ENCODER_HPP
#define CAN_SMBUS_SRC_ABS_ENCODER_HPP

#include "core/cs_manager.h"
#include "core/cs_send.h"
#include "core/cs_reg_share.h"
#include "core/cs_reg_signal.h"

#include "type.hpp"

namespace can_smbus
{

struct abs_encoder_param_t
{
    int id;
    DirectRota_t direction;
    Interval_t interval;
    Gateway_t gw_id;
};

class AbsEncoder
{
private:
    CSManager_t m_manager;

    Interval_t m_interval;

    CSRegShare_t r_var;

    int m_direction_num;

public:
    AbsEncoder(void);
    AbsEncoder(int id, DirectRota_t direction = DirectRota_FORWARD, Interval_t interval = Interval_20Hz, Gateway_t gw_id = Gateway_1);
    AbsEncoder(struct abs_encoder_param_t* param);

    void AbsEncoder_cons(int id, DirectRota_t direction = DirectRota_FORWARD, Interval_t interval = Interval_20Hz, Gateway_t gw_id = Gateway_1);
    void AbsEncoder_cons(struct abs_encoder_param_t *param);

    void init(void);

    float angle(void);
};

}

#endif /*CAN_SMBUS_SRC_ABS_ENCODER_HPP*/