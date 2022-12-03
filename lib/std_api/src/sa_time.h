#ifndef STD_API_SRC_SA_TIME_H
#define STD_API_SRC_SA_TIME_H

#include "system/sa_type.h"
#include <sys/time.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    struct timespec befo_time;
} SATime_timer_t;

void SATime_timerStart(SATime_timer_t* tim);
uint64_t SATime_timerGetUs(SATime_timer_t* tim);
uint64_t SATime_timerGetMs(SATime_timer_t* tim);
float SATime_timerGetSec(SATime_timer_t* tim);


void SATime_delaySec(float second);
void SATime_delayUs(uint64_t us);
void SATime_delayMs(uint64_t ms);

uint64_t SATime_getUs(void);
uint64_t SATime_getMs(void);
uint64_t SATime_getSec(void);

void SATime_timespecGetnow(struct timespec* res);
void SATime_timespecAdd(const struct timespec* A, const struct timespec* B, struct timespec* res);
void SATime_timespecSub(const struct timespec* A, const struct timespec* B, struct timespec* res);
void SATime_timespecAddMs(struct timespec* res, uint64_t ms);

#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_TIME_H*/