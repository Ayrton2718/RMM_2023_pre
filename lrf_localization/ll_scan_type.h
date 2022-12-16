#ifndef LRF_LOCALIZATION_SRC_LL_SCAN_TYPE_H
#define LRF_LOCALIZATION_SRC_LL_SCAN_TYPE_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include <std_api/std_api.h>

#include "ll_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float angle;
    float range;
} LLScanType_scanInfo_t;

typedef struct
{
    size_t count;
    SABuffer_t scans;

    uint64_t time_stamp;
} LLScanType_t;



LLScanType_t LLScanType_create(LLTimer_t timer);
void LLScanType_destructor(LLScanType_t* scan);

LLScanType_t LLScanType_clone(LLScanType_t* scan);

size_t LLScanType_count(LLScanType_t* scan);

void LLScanType_set(LLScanType_t* scan, float rad, float dist);
float LLScanType_getRad(LLScanType_t* scan, size_t i);
float LLScanType_getDist(LLScanType_t* scan, size_t i);

void LLScanType_sort(LLScanType_t* scan);

void LLScanType_show(LLScanType_t* scan);

void LLScanType_diffLaserScan(LLScanType_t* scan1, LLScanType_t* scan2);

#ifdef __cplusplus
}
#endif

#endif /*LRF_LOCALIZATION_SRC_LL_SCAN_TYPE_H*/
