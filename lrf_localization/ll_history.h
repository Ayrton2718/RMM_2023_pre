#ifndef LRF_LOCALIZATION_SRC_LL_HISTORY_H
#define LRF_LOCALIZATION_SRC_LL_HISTORY_H

#include "ll_type.h"
#include "ll_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LLHistory_t;

LLHistory_t LLHistory_create(LLTimer_t timer, LLType_pos_t init_pos);

void LLHistory_addDeltaMove(LLHistory_t his, LLType_pos_t d_move);

void LLHistory_update(LLHistory_t his, LLType_pos_t pos, uint64_t timer);

LLType_pos_t LLHistory_calcPos(LLHistory_t his);

LLType_pos_t LLHistory_getDelta(LLHistory_t his, uint64_t timer);


#ifdef __cplusplus
}
#endif

#endif /*LRF_LOCALIZATION_SRC_LL_HISTORY_H*/