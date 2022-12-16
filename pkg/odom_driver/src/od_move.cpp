#include "od_move.hpp"

namespace odom_driver
{

Move::Move(void)
{
}

Move::Move(const MoveParam_t* param)
{
    this->Move_cons(param);
}

void Move::Move_cons(const MoveParam_t* param)
{
    mot[0].LPMotor_cons(&param->motor_param[0]);
    mot[1].LPMotor_cons(&param->motor_param[1]);
    mot[2].LPMotor_cons(&param->motor_param[2]);
    mot[3].LPMotor_cons(&param->motor_param[3]);
}

void Move::init(void)
{
    mot[0].init();
    mot[1].init();
    mot[2].init();
    mot[3].init();
}

void Move::move(Vector_t vec)
{
    if(30 < vec.theta)
    {
        vec.theta = 30;
    }else if(vec.theta < -30){
        vec.theta = -30;
    }

    float Power1 = vec.x + vec.y;// + vec.theta;
    float Power2 = vec.x - vec.y;// + vec.theta;
    float Power3 =  -vec.x - vec.y;// + vec.theta;
    float Power4 =  -vec.x + vec.y;// + vec.theta;

    mot[0].set_power(Power1);
    mot[1].set_power(Power2);
    mot[2].set_power(Power3);
    mot[3].set_power(Power4);
}

}