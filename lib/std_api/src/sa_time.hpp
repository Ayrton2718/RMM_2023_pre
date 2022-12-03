#ifndef STD_API_SRC_SA_TIME_HPP
#define STD_API_SRC_SA_TIME_HPP

#include "sa_time.h"

namespace std_api
{

class Delay
{
public:
    static void sec(float second);
    static void ms(uint64_t ms);
    static void us(uint64_t us);
};


class GetNow
{
public:
    static uint64_t sec(void);
    static uint64_t ms(void);
    static uint64_t us(void);
};


class Timer
{
private:
    SATime_timer_t m_timer;

public:
    Timer();

    void start(void);
    void restart(void);

    float getSec(void);
    uint64_t getMs(void);
    uint64_t getUs(void);
};

}

#endif /*STD_API_SRC_SA_TIME_HPP*/