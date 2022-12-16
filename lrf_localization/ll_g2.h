#ifndef LL_LRF_SIM

#ifndef LRF_LOCALIZATION_SRC_LL_G2_H
#define LRF_LOCALIZATION_SRC_LL_G2_H

#include <std_api/std_api.h>

#include "ll_type.h"
#include "ll_scan_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LLG2_t;

LLG2_t LLG2_create(const char* comm_path, LLTimer_t timer);

void LLG2_stop(LLG2_t obj);

LLScanType_t LLG2_getValue(LLG2_t obj);

#ifdef __cplusplus
}
#endif

#endif /*LRF_LOCALIZATION_SRC_LL_G2_H*/

#endif /*LL_LRF_SIM*/