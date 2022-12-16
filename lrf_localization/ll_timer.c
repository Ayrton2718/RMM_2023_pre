#include "ll_timer.h"

LLTimer_t LLTimer_create(void)
{
    LLTimer_t tim;
    SATime_timerStart(&tim);
    return tim;
}

uint64_t LLTimer_getNow(LLTimer_t obj)
{
    return SATime_timerGetMs(&obj);
}