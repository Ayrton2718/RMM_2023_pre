#ifndef ODOM_DRIVER_SRC_OD_CONTROLLER_HPP
#define ODOM_DRIVER_SRC_OD_CONTROLLER_HPP

#include <can_smbus/can_smbus.hpp>
#include <math.h>

#include "od_localization.hpp"
#include "od_move.hpp"
#include "od_navigation.hpp"

namespace odom_driver
{

typedef struct
{
    LocalizationParam_t loc_param;
    MoveParam_t         move_param;
} AutoParam_t;


class Auto
{
private:

public:
    Auto(void);
    Auto(const LocalizationParam_t* param);

    void Auto_cons(const LocalizationParam_t* param);

    void init(void);

    void set_nav(Navigation* nav);

    Vector_t get_pos(void);
    bool is_arrive(void);


    void deamon_start(void);
};

}

/*  How to use.
#include <odom_driver/odom_driver.hpp>

int main(void)
{
    odom_driver::AutoParam_t param;

    // ~ set params ~

    odom_driver::Auto auto;
    auto.init();
    auto.deamon_start();

    while(1)
    {
        NavigationLine line_nav(100, 0);
        auto.set_nav(&line_nav);
        while(auto.is_arrive() == false)
        {
            std_api::Delay::ms(100);
        }

        NavigationCircle circle_nav(10, 0);
        auto.set_nav(&circle_nav);
        while(auto.is_arrive() == false)
        {
            std_api::Delay::ms(100);
        }
    }
    
}
*/

#endif /*ODOM_DRIVER_SRC_OD_CONTROLLER_HPP*/