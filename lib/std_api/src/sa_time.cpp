#include "sa_time.hpp"

namespace std_api
{

void Delay::sec(float second)
{
    SATime_delaySec(second);
}

void Delay::ms(uint64_t us)
{
    SATime_delayMs(us);
}

void Delay::us(uint64_t us)
{
    SATime_delayUs(us);
}


uint64_t GetNow::sec(void)
{
    return SATime_getSec();
}

uint64_t GetNow::ms(void)
{
    return SATime_getMs();
}

uint64_t GetNow::us(void)
{
    return SATime_getUs();
}


Timer::Timer(void)
{
}

void Timer::start(void)
{
    SATime_timerStart(&this->m_timer);
}

void Timer::restart(void)
{
    SATime_timerStart(&this->m_timer);
}

uint64_t Timer::getUs(void)
{
    return SATime_timerGetUs(&this->m_timer);
}

uint64_t Timer::getMs(void)
{
    return SATime_timerGetMs(&this->m_timer);
}

float Timer::getSec(void)
{
    return SATime_timerGetSec(&this->m_timer);
}

}