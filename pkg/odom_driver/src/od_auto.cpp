#include "od_auto.hpp"


namespace odom_driver
{

Auto::Auto(void)
{
}

Auto::Auto(const AutoParam_t* param)
{
    this->Auto_cons(param);
}

Auto::~Auto(void)
{
}

void Auto::Auto_cons(const AutoParam_t* param)
{
    this->m_local = param->localization;
    m_move.Move_cons(&param->move_param);
    m_kp = param->kp_param;
    m_ki = param->ki_param;
    m_kd = param->kd_param;
    m_nav = NavigationLine::create(0, 0, 0, 0);
    m_nav->init();

    this->m_befo_diff.x = 0;
    this->m_befo_diff.y = 0;
    this->m_befo_diff.theta = 0;

    this->m_before_place.x = 0;
    this->m_before_place.y = 0;
    this->m_before_place.theta = 0;

    this->m_integral.x = 0;
    this->m_integral.y = 0;
    this->m_integral.theta = 0;

    pthread_mutex_init(&this->m_locker, NULL);
}

void Auto::init(void)
{
    m_local->init();
    m_move.init();
    m_tim.start();
}

void Auto::set_nav(Navigation* nav)
{
    SALOCKER_LOCK(&this->m_locker);
    Vector_t fin_pos = this-> m_nav->finalcoordinates();
    m_before_place.x += fin_pos.x;
    m_before_place.y += fin_pos.y;
    m_before_place.theta +=fin_pos.theta;

    delete m_nav;
    m_nav = nav;
    m_nav->init();
    m_arrive_flag = false;
    SALOCKER_UNLOCK(&this->m_locker);
}

Vector_t Auto::get_pos(void)
{
    return m_local->estimate();
}

bool Auto::is_arrive(void)
{
    bool arrive_flag;
    SALOCKER_LOCK(&this->m_locker);
    arrive_flag = this->m_arrive_flag;
    SALOCKER_UNLOCK(&this->m_locker);
    return arrive_flag;
}

void Auto::daemon_start(void)
{
    pthread_t thread;
    SATHREAD_CREATE("od_auto", &thread, NULL, Auto::daemon, this);
}

void Auto::run(void)
{
    float diff_time = (float)m_tim.getMs() / 1000.0;
    m_tim.restart();

    Vector_t base_pos;
    Vector_t target;
    SALOCKER_LOCK(&this->m_locker);
    base_pos = this->m_before_place;
    target = m_nav->generate();
    SALOCKER_UNLOCK(&this->m_locker);

    target.x += base_pos.x;
    target.y += base_pos.y;
    target.theta += base_pos.theta;

    Vector_t now_pos = this->get_pos();

    //now
    Vector_t now_diff;
    now_diff.x = target.x - now_pos.x;
    now_diff.y = target.y - now_pos.y;
    now_diff.theta = target.theta - (now_pos.theta * 180 / M_PI);

    m_integral.x += (now_diff.x + this->m_befo_diff.x) / 2.0 * diff_time;
    m_integral.y += (now_diff.y + this->m_befo_diff.y) / 2.0 * diff_time;
    m_integral.theta += (now_diff.theta + this->m_befo_diff.theta) / 2.0 * diff_time;

    const float max_integral = 20 / m_ki;
    if(max_integral < m_integral.x)
    {
        m_integral.x = max_integral;
    }else if(m_integral.x < -max_integral){
        m_integral.x = -max_integral;
    }

    if(max_integral < m_integral.y)
    {
        m_integral.y = max_integral;
    }else if(m_integral.y < -max_integral){
        m_integral.y = -max_integral;
    }

    if(max_integral < m_integral.theta)
    {
        m_integral.theta = max_integral;
    }else if(m_integral.theta < -max_integral){
        m_integral.theta = -max_integral;
    }

    //P制御
    Vector_t p;
    p.x =  (now_diff.x) * cos(now_pos.theta) + (now_diff.y) * sin(now_pos.theta);
    p.y = -(now_diff.x) * sin(now_pos.theta) + (now_diff.y) * cos(now_pos.theta);
    p.theta = now_diff.theta;

    //I制御
    Vector_t i;
    i.x = m_integral.x;
    i.y = m_integral.y; 
    i.theta = m_integral.theta;

    //D制御
    Vector_t d;
    d.x = (now_diff.x - this->m_befo_diff.x) / diff_time;
    d.y = (now_diff.y - this->m_befo_diff.y) / diff_time;
    d.theta = (now_diff.theta - this->m_befo_diff.theta) / diff_time;

    Vector_t power;
    power.x = (m_kp * p.x) + (m_ki * i.x) + (m_kd * d.x);
    power.y = (m_kp * p.y) + (m_ki * i.y) + (m_kd * d.y);
    power.theta = (m_kp * p.theta) + (m_ki * i.theta) + (m_kd * d.theta);

    m_move.move(power);

    if(m_nav->is_finish() == true && fabsf(now_diff.x) <= 3 && fabsf(now_diff.y) <= 3 && fabsf(now_diff.theta) <= 2) 
    {
        SALOCKER_LOCK(&this->m_locker);
        m_arrive_flag = true;
        SALOCKER_UNLOCK(&this->m_locker);
    }
    else
    {
        SALOCKER_LOCK(&this->m_locker);
        m_arrive_flag = false;
        SALOCKER_UNLOCK(&this->m_locker);
    }

    this->m_befo_diff = now_diff;
}

void* Auto::daemon(void* arg)
{
    Auto* auto_run = (Auto *)arg;

    while(true)
    {
        auto_run->run();
        std_api::Delay::ms(50);
    }
}

}