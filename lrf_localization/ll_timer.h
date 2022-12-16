#ifndef LRF_LOCALIZATION_SRC_LL_TIMER_H
#define LRF_LOCALIZATION_SRC_LL_TIMER_H

#include <std_api/std_api.h>
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SATime_timer_t LLTimer_t;

// ms
LLTimer_t LLTimer_create(void);

uint64_t LLTimer_getNow(LLTimer_t obj);

#ifdef __cplusplus
}
#endif

#endif /*LRF_LOCALIZATION_SRC_LL_TIMER_H*/