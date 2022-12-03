#include "sa_time.h"


void SATime_timerStart(SATime_timer_t* tim)
{
    SATime_timespecGetnow(&tim->befo_time);
}

uint64_t SATime_timerGetUs(SATime_timer_t* tim)
{
    struct timespec now;
    struct timespec diff;
    SATime_timespecGetnow(&now);
    SATime_timespecSub(&now, &tim->befo_time, &diff);
    uint64_t now_ms = diff.tv_sec * (uint64_t)1e6;
    now_ms += (diff.tv_nsec / (uint64_t)1e3);
    return now_ms;
}

uint64_t SATime_timerGetMs(SATime_timer_t* tim)
{
    struct timespec now;
    struct timespec diff;
    SATime_timespecGetnow(&now);
    SATime_timespecSub(&now, &tim->befo_time, &diff);
    uint64_t now_ms = diff.tv_sec * (uint64_t)1e3;
    now_ms += (diff.tv_nsec / (uint64_t)1e6);
    return now_ms;
}

float SATime_timerGetSec(SATime_timer_t* tim)
{
    struct timespec now;
    struct timespec diff;
    SATime_timespecGetnow(&now);
    SATime_timespecSub(&now, &tim->befo_time, &diff);
    float now_ms = (float)diff.tv_sec;
    now_ms += (float)diff.tv_nsec / (float)1e9;
    return now_ms;
}


void SATime_delaySec(float second)
{
    struct timespec ts;
    ts.tv_sec = (time_t)second;
    ts.tv_nsec = (uint64_t)((second - (float)ts.tv_sec) * (float)1e9);

    nanosleep(&ts, NULL);
}

void SATime_delayUs(uint64_t us)
{
    struct timespec ts;
    ts.tv_sec = us / (uint64_t)1e6;
    ts.tv_nsec = (us % (uint64_t)1e6) * (uint64_t)1e3;
    nanosleep(&ts, NULL);
}

void SATime_delayMs(uint64_t ms)
{
    struct timespec ts;
    ts.tv_sec = ms / (uint64_t)1e3;
    ts.tv_nsec = (ms % (uint64_t)1e3) * 1e6;
    nanosleep(&ts, NULL);
}


uint64_t SATime_getUs(void)
{
    struct timespec now;
    SATime_timespecGetnow(&now);
    uint64_t now_ms = now.tv_sec * (uint64_t)1e6;
    now_ms += (now.tv_nsec / (uint64_t)1e3);
    return now_ms;
}

uint64_t SATime_getMs(void)
{
    struct timespec now;
    SATime_timespecGetnow(&now);
    uint64_t now_ms = now.tv_sec * (uint64_t)1e3;
    now_ms += (now.tv_nsec / (uint64_t)1e6);
    return now_ms;
}

uint64_t SATime_getSec(void)
{
    struct timespec now;
    SATime_timespecGetnow(&now);
    return now.tv_sec;
}


void SATime_timespecGetnow(struct timespec* res)
{
    clock_gettime(CLOCK_REALTIME, res);
}

void SATime_timespecAdd(const struct timespec* A, const struct timespec* B, struct timespec* res)
{
    res->tv_sec  = A->tv_sec  + B->tv_sec;
    res->tv_nsec = A->tv_nsec + B->tv_nsec;
    if((uint64_t)1e9 <= res->tv_nsec)
    {
        res->tv_sec++;
        res->tv_nsec -= (uint64_t)1e9;
    }
}

void SATime_timespecSub(const struct timespec* A, const struct timespec* B, struct timespec* res)
{
    res->tv_sec  = A->tv_sec  - B->tv_sec;
    if(A->tv_nsec < B->tv_nsec)
    {
        res->tv_sec--;
        res->tv_nsec = A->tv_nsec + (uint64_t)1e9;
        res->tv_nsec -= B->tv_nsec;
    }else{
        res->tv_nsec = A->tv_nsec - B->tv_nsec;
    }
}

void SATime_timespecAddMs(struct timespec* res, uint64_t ms)
{
    clock_gettime(CLOCK_REALTIME, res);

    res->tv_sec += ms / (uint64_t)1e3;
    res->tv_nsec += (ms % (uint64_t)1e3) * (uint64_t)1e6;
    if((uint64_t)1e9 <= res->tv_nsec)
    {
        res->tv_sec++;
        res->tv_nsec -= (uint64_t)1e9;
    }
}