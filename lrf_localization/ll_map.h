#ifndef LRF_PF_LL_MAP_H
#define LRF_PF_LL_MAP_H


#include "ll_type.h"
#include "ll_scan_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LPMAP_PIXEL_METER ((float)0.010)

typedef enum
{
    LLMap_writeType_Laser,
    LLMap_writeType_Object,
    LLMap_writeType_Support
} LLMap_writeType_t;


typedef void* LLMap_t;

 // mm? cm?
LLMap_t LLMap_create(float x, float y, float offset_x, float offset_y);

LLMap_t LLMap_copy(LLMap_t map);

void LLMap_destructor(LLMap_t map);

void LLMap_addRectangle(LLMap_t obj, float x0, float y0, float x1, float y1, float tick, LLMap_writeType_t write_type);

void LLMap_addBox(LLMap_t obj, float x0, float y0, float half_width, float half_height, LLMap_writeType_t write_type);

void LLMap_addCircle(LLMap_t obj, float center_x, float center_y, float radius, LLMap_writeType_t write_type);

void LLMap_addLine(LLMap_t obj, float start_x, float start_y, float end_x, float end_y, LLMap_writeType_t write_type);

void LLMap_addDot(LLMap_t obj, float x, float y, LLMap_writeType_t write_type);


void LLMap_removeLines(LLMap_t obj);

void LLMap_show(LLMap_t obj, const char* tag, LLType_pos_t* pos);
void LLMap_showWait(LLMap_t obj, const char* tag);


void LLMap_writeScan(LLMap_t obj, float x0, float y0, float theta, LLScanType_t scan, LLMap_writeType_t write_type);


size_t LLMap_width(LLMap_t obj);
size_t LLMap_height(LLMap_t obj);

#ifdef __cplusplus
}
#endif

#endif /*LRF_PF_LL_MAP_H*/
 