#include "lib/library.hpp"
#include "pkg/package.hpp"

#include "lrf_localization/ll_local_lrf.hpp"
#include "webgl_map/wm_gl.h"

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


// #include "ap_axi_sdata.h"
// #include "hls_stream.h"
// #include <math.h>
// #include <stdint.h>

// #define LPTOMASCUDA_EPSIRON ((float)10e-6)

// typedef union
// {
// 	float f;
// 	uint32_t ui;
// } Float2Int_t;


// void DMA_powpow(hls::stream< ap_axis<32,2,5,6> > &A,
// 	     hls::stream< ap_axis<32,2,5,6> > &B)
// {
// #pragma HLS INTERFACE axis port=A
// #pragma HLS INTERFACE axis port=B
// #pragma HLS interface s_axilite port=return

// 	ap_axis<32,2,5,6> tmp;

// 	Float2Int_t my_x;
// 	Float2Int_t my_y;
// 	Float2Int_t my_yaw;
// 	Float2Int_t target_x0;
// 	Float2Int_t target_y0;
// 	Float2Int_t target_vx;
// 	Float2Int_t target_vy;

// 	A.read(tmp);
// 	my_x.ui = tmp.data;
// 	A.read(tmp);
// 	my_y.ui = tmp.data;
// 	A.read(tmp);
// 	my_yaw.ui = tmp.data;
// 	A.read(tmp);
// 	target_x0.ui = tmp.data;
// 	A.read(tmp);
// 	target_y0.ui = tmp.data;
// 	A.read(tmp);
// 	target_vx.ui = tmp.data;
// 	A.read(tmp);
// 	target_vy.ui = tmp.data;

// 	float dir_x = cosf(my_yaw.f) * -1;
// 	float dir_y = sinf(my_yaw.f) * -1;

// 	float d_x = my_x.f - target_x0.f;
// 	float d_y = my_y.f - target_y0.f;

// 	float donom = target_vx.f * dir_y - target_vy.f * dir_x;
// 	uint8_t isin_donom = (1 < fabs(donom) / LPTOMASCUDA_EPSIRON)? 1:0;
// 	donom = isin_donom * donom + (1 - isin_donom) * 1;

// 	float u = (d_x * dir_y - d_y * dir_x) / donom;
// 	uint8_t isin_u = ((0 <= u)? 1:0) &  ((u <= 1)? 1:0);

// 	Float2Int_t t;
// 	t.f = (target_vx.f * d_y - target_vy.f * d_x) / donom;

// 	t.f = isin_donom * t.f;
// 	t.f = isin_u * t.f;
// 	tmp.data = t.ui;
// 	B.write(tmp);
// }
