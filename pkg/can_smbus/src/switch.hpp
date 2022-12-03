#ifndef CAN_SMBUS_SRC_SWITCH_HPP
#define CAN_SMBUS_SRC_SWITCH_HPP

#include "core/cs_manager.h"
#include "core/cs_send.h"
#include "core/cs_reg_share.h"
#include "core/cs_reg_signal.h"

#include "type.hpp"

namespace can_smbus
{

struct switch_param_t
{
    int id;
    Port_t port;
    Interval_t interval;
    Gateway_t gw_id;
};

class Switch
{
private:
    CSManager_t m_manager;

    CSRegShare_t r_all_state;

    Interval_t m_interval;

    uint8_t m_port;

public:
    Switch(void);
    Switch(int id, Port_t port, Interval_t interval = Interval_20Hz, Gateway_t gw_id = Gateway_1);
    Switch(struct switch_param_t* param);

    void Switch_cons(int id, Port_t port, Interval_t interval = Interval_20Hz, Gateway_t gw_id = Gateway_1);
    void Switch_cons(struct switch_param_t *param);

    void init(void);

    bool get(void);
};

}

#endif /*CAN_SMBUS_SRC_SWITCH_HPP*/