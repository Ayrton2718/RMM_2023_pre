#include "lib/library.hpp"
#include "pkg/package.hpp"


int main(void)
{
    LIBInitializer_init("me", "/tmp/rmm_RobotRMM_2022_current");
    PKGInitializer_init(SABOOL_TRUE, 1);
    
    can_smbus::connectionWait(0);
    can_smbus::safetyOff();

    can_smbus::LPMotor mot(1, can_smbus::Port_1);

    SALOG_CREATE_FILE("main");

    while(1)
    {
        SALOG_INFO("main", "Hello");
        std_api::Delay::ms(100);
    }
}
