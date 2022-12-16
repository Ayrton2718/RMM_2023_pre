#ifndef LRF_LOCALIZATION_SRC_LL_PF_H
#define LRF_LOCALIZATION_SRC_LL_PF_H

#include "ll_type.h"
#include "ll_scan_type.h"
#include "ll_tomas.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LLPF_NODE_COUNT (80)

//  create
//  ↓
//  loop:
//      predict
//      ↓
//      simulation
//      ↓
//      estimate
//      ↓
//      resampling

typedef void* LLPf_t;


// LLPf_t LLPf_create(void);
LLPf_t LLPf_create(const LLType_pos_t* init_pos, LLTomas_t tomas);

// for predict
LLType_pos_t LLPf_predict(LLPf_t obj, const LLType_pos_t* d_move, LLScanType_t* scan);

// calculation
// LLType_pos_t LLPf_estimate();

// for resampling
// void LLPf_resampling(LLPf_t obj);

// void LLPf_showAll(LLPf_t obj);

// void LLPf_simulation(LLPf_t obj, LLType_laserScan_t* scan);

// void LLPf_sampleOne(LLPf_t obj, LLType_pos_t pos);


#ifdef __cplusplus
}
#endif

#endif /*LRF_LOCALIZATION_SRC_LL_PF_H*/