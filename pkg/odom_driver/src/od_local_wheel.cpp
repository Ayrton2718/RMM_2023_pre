#include "od_local_wheel.hpp"

namespace odom_driver
{

LocalWheel::LocalWheel(void)
{
}

LocalWheel::LocalWheel(const LocalWheelParam_t* param)
{
    this->LocalWheel_cons(param);
}

void LocalWheel::LocalWheel_cons(const LocalWheelParam_t* param)
{
    m_vec_encX = param->vec_encX;
    m_vec_encY = param->vec_encY;

    m_mpr = param->mpr;

    m_encX.Encoder_cons(&param->encX_param);
    m_encY.Encoder_cons(&param->encY_param);
    m_gyro.Gyro_cons(&param->gyro_param);

    m_encX_ppr = param->encX_param.ppr;
    m_encY_ppr = param->encY_param.ppr;

    this->m_daemon_running = false;
    SALOCKER_INIT(&this->m_locker, NULL);
}

void LocalWheel::init(void)
{
    m_encX.init();
    m_encY.init();
    m_gyro.init();
}

Vector_t LocalWheel::estimate(void)
{
    if(this->m_daemon_running == false)
    {
        this->run();
    }

    Vector_t now_pos;

    SALOCKER_LOCK(&this->m_locker);
    now_pos = this->m_now_estimate;
    SALOCKER_UNLOCK(&this->m_locker);
    
    return now_pos;
}

void LocalWheel::daemon_start(void)
{
    this->m_daemon_running = true;

    pthread_t thread;
    SATHREAD_CREATE("od_local_wheel", &thread, NULL, LocalWheel::daemon, this);
}


void LocalWheel::run(void)
{
    float encX = this->m_encX.getAsInt() / cos(this->m_vec_encX.theta*M_PI/180.0);
    float encY = this->m_encY.getAsInt() / cos(this->m_vec_encY.theta*M_PI/180.0);
    float yaw_rad = -this->m_gyro.get() * M_PI / 180.0;

    // static float distanceX = (this->m_vec_encX.y - this->m_vec_encY.y);
    // static float distanceY = (this->m_vec_encX.x - this->m_vec_encY.x);

    float E_x = (encX / (float)this->m_encX_ppr) * M_PI * (2.0 * this->m_mpr);
    float E_y = (encY / (float)this->m_encY_ppr) * M_PI * (2.0 * this->m_mpr);

    float turn_x = yaw_rad * this->m_vec_encX.y;// * distanceX; //
    float turn_y = -yaw_rad * this->m_vec_encY.x;// * distanceY; //

    float DE_x = (E_x - turn_x) - (this->m_last_E_x - this->m_last_turn_x);
    float DE_y = (E_y - turn_y) - (this->m_last_E_y - this->m_last_turn_y);

    float D_x = (DE_x * cos(yaw_rad)) + (DE_y * sin(yaw_rad)); //
    float D_y = (DE_y * cos(yaw_rad)) - (DE_x * sin(yaw_rad)); //

    this->m_sum_x += D_x;
    this->m_sum_y += D_y;

    this->m_last_E_x = E_x;
    this->m_last_E_y = E_y;
    this->m_last_turn_x = turn_x;
    this->m_last_turn_y = turn_y;

    SALOCKER_LOCK(&this->m_locker);
    this->m_now_estimate.x = this->m_sum_x;
    this->m_now_estimate.y = this->m_sum_y;
    this->m_now_estimate.theta = yaw_rad;
    SALOCKER_UNLOCK(&this->m_locker);

    SALOG_INFO("od_localization", "raw=(%f, %f, %f), est=(%f, %f, %f)", encX, encY, yaw_rad, this->m_sum_x, this->m_sum_y, yaw_rad);
}

void* LocalWheel::daemon(void* arg)
{
    LocalWheel* me = (LocalWheel*)arg;

    while(1)
    {
        me->run();
        std_api::Delay::ms(20);
    }
}

}