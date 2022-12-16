#include "lib/library.hpp"
#include "pkg/package.hpp"
#include "lrf_localization/ll_local_lrf.hpp"

int main(void)
{
    LIBInitializer_init("me", "/tmp/rmm_RobotRMM_2022_current");
    PKGInitializer_init(SABOOL_TRUE, 1);

    SALOG_CREATE_FILE("od_localization");
    SALOG_CREATE_FILE("ll_history");
    SALOG_CREATE_FILE("auto");
    SALOG_CREATE_FILE("lrf_localization");
    SALOG_CREATE_FILE("ll_map");
    SALOG_CREATE_FILE("main");

    lrf_localization::LocalLrfParam_t local_param;
    local_param.init_x = 0;
    local_param.init_y = -0.5;
    local_param.init_teta = 0;

    local_param.wheel_param.vec_encX.x = 0;
    local_param.wheel_param.vec_encX.y = 0.35;
    local_param.wheel_param.vec_encX.theta = 180;

    local_param.wheel_param.vec_encY.x = 0.35;
    local_param.wheel_param.vec_encY.y = 0;
    local_param.wheel_param.vec_encY.theta = 180;

    local_param.wheel_param.mpr = 0.03;

    local_param.wheel_param.encX_param.id = 1;
    local_param.wheel_param.encX_param.mode = can_smbus::EncoderMode_Count;
    local_param.wheel_param.encX_param.direction = can_smbus::DirectRota_INVERSE;
    local_param.wheel_param.encX_param.interval = can_smbus::Interval_50Hz;
    local_param.wheel_param.encX_param.ppr = can_smbus::EncoderPPR_100;
    local_param.wheel_param.encX_param.gw_id = can_smbus::Gateway_1;

    local_param.wheel_param.encY_param.id = 0;
    local_param.wheel_param.encY_param.mode = can_smbus::EncoderMode_Count;
    local_param.wheel_param.encY_param.direction = can_smbus::DirectRota_FORWARD;
    local_param.wheel_param.encY_param.interval = can_smbus::Interval_50Hz;
    local_param.wheel_param.encY_param.ppr = can_smbus::EncoderPPR_100;
    local_param.wheel_param.encY_param.gw_id = can_smbus::Gateway_1;

    local_param.wheel_param.gyro_param.id = 0;
    local_param.wheel_param.gyro_param.mode = can_smbus::GyroMode_Yaw;
    local_param.wheel_param.gyro_param.interval = can_smbus::Interval_50Hz;
    local_param.wheel_param.gyro_param.gw_id = can_smbus::Gateway_1;
    
    odom_driver::MoveParam_t move_param;
    // move_param.motor_param[0].id = 1;
    // move_param.motor_param[0].port = can_smbus::Port_2;
    // move_param.motor_param[0].direction = can_smbus::DirectRota_INVERSE;
    // move_param.motor_param[0].gw_id = can_smbus::Gateway_1;

    // move_param.motor_param[1].id = 0;
    // move_param.motor_param[1].port = can_smbus::Port_1;
    // move_param.motor_param[1].direction = can_smbus::DirectRota_INVERSE;
    // move_param.motor_param[1].gw_id = can_smbus::Gateway_1;

    // move_param.motor_param[2].id = 0;
    // move_param.motor_param[2].port = can_smbus::Port_2;
    // move_param.motor_param[2].direction = can_smbus::DirectRota_INVERSE;
    // move_param.motor_param[2].gw_id = can_smbus::Gateway_1;

    // move_param.motor_param[3].id = 1;
    // move_param.motor_param[3].port = can_smbus::Port_1;
    // move_param.motor_param[3].direction = can_smbus::DirectRota_INVERSE;
    // move_param.motor_param[3].gw_id = can_smbus::Gateway_1;


    lrf_localization::LocalLrf local;
    local.LocalLrf_cons(&local_param);
    // odom_driver::Move move(&move_param);

    local.init();
    // move.init();

    local.daemon_start();

    while(1)
    {
        odom_driver::Vector_t vec = local.estimate();
        SALOG_INFO("main", "%f, %f, %f", vec.x, vec.y, vec.theta * 180 / M_PI);

        // vec.x -= CCDouble_get(g_target_x);
        // vec.y -= CCDouble_get(g_target_y);
        // vec.theta -= CCDouble_get(g_target_z);

        vec.x *= -50;
        vec.y *= -50;
        vec.theta *= -180 / M_PI;
        // move.move(vec);
        std_api::Delay::ms(50);
    }
}
