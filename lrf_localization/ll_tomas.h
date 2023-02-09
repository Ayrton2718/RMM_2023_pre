#ifndef LRF_LOCALIZATION_SRC_LL_TOMAS_H
#define LRF_LOCALIZATION_SRC_LL_TOMAS_H

#include "ll_type.h"
#include "ll_scan_type.h"

// #define ENABLE_FPGA

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LLTomas_t;

LLTomas_t LLTomas_create(LLTimer_t timer);

LLTomas_t LLTomas_clone(LLTomas_t obj);

void LLTomas_drawLine(LLTomas_t obj, float x0, float y0, float x1, float y1);
void LLTomas_drawBox(LLTomas_t obj, float x0, float y0, float half_width, float half_height);

// SABool_t LLTomas_isContainBox(LLTomas_t obj, float x0, float y0, float theta, float angle);

LLScanType_t LLTomas_virtualScan(LLTomas_t obj, float x0, float y0, float theta, float angle_increment, float angle_from, float angle1_to);

#ifdef ENABLE_FPGA
void LLTomas_init(void);

LLScanType_t LLTomas_simulateRange(LLTomas_t obj, float x0, float y0, float theta, LLScanType_t* scan);
#else /*ENABLE_FPGA*/
LLScanType_t LLTomas_simulateRange(LLTomas_t obj, float x0, float y0, float theta, LLScanType_t* scan);
#endif /*ENABLE_FPGA*/

#ifdef __cplusplus
}
#endif

#endif /*LRF_LOCALIZATION_SRC_LL_TOMAS_H*/
