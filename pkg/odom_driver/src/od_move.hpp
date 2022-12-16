#ifndef ODOM_DRIVER_SRC_OD_MOVE_HPP
#define ODOM_DRIVER_SRC_OD_MOVE_HPP

#include <can_smbus/can_smbus.hpp>
#include <std_api/std_api.hpp>
#include "od_type.hpp"

namespace odom_driver
{

typedef struct
{
    struct can_smbus::lp_motor_param_t motor_param[4];
} MoveParam_t;

class Move
{
private:
    can_smbus::LPMotor mot[4];

public:
    Move(void);
    Move(const MoveParam_t* param);

    void Move_cons(const MoveParam_t* param);

    void init(void);

    void move(Vector_t vec);
};

}

#endif /*ODOM_DRIVER_SRC_OD_MOVE_HPP*/