#include "ll_tomas.h"

#include <math.h>
#include <float.h>

#include <stdio.h>
#include <math.h>
#include <stdint.h>

#ifdef ENABLE_FPGA
#include <pynq_api.h>
#endif /*ENABLE_FPGA*/

#ifndef M_PI
#define M_PI 3.14159263
#endif /*M_PI*/

typedef struct
{
    float x;
    float y;
} LLTomas_vector_t;


typedef struct
{
    float x0;
    float y0;
    LLTomas_vector_t vec;
} LLTomas_objLine_t;

typedef struct
{
    SABuffer_t objs; 

    LLTimer_t timer;
} LLTomas_info_t;


LLTomas_t LLTomas_create(LLTimer_t timer)
{
    LLTomas_info_t* _obj = (LLTomas_info_t*)SAMEM_MALLOC(sizeof(LLTomas_info_t));
    SABuffer_create(&_obj->objs, sizeof(LLTomas_objLine_t));
    _obj->timer = timer;

    #ifdef ENABLE_FPGA
    LLTomas_init();
    #endif /*ENABLE_FPGA*/
    return _obj;
}

LLTomas_t LLTomas_clone(LLTomas_t obj)
{
    LLTomas_info_t* _obj = (LLTomas_info_t*)obj;

    LLTomas_info_t* _cloned_obj = (LLTomas_info_t*)SAMEM_MALLOC(sizeof(LLTomas_info_t));
    SABuffer_clone(&_obj->objs, &_cloned_obj->objs);
    _cloned_obj->timer = _obj->timer;
    return _cloned_obj;
}


void LLTomas_drawLine(LLTomas_t obj, float x0, float y0, float x1, float y1)
{
    LLTomas_info_t* _obj = (LLTomas_info_t*)obj;
    
    LLTomas_objLine_t line_obj;
    line_obj.x0 = x0;
    line_obj.y0 = y0;
    line_obj.vec.x = x1 - x0;
    line_obj.vec.y = y1 - y0;

    SABuffer_add(&_obj->objs, &line_obj);
}

void LLTomas_drawBox(LLTomas_t obj, float x0, float y0, float half_width, float half_height)
{
    LLTomas_drawLine(obj, x0 - half_width, y0 - half_height, x0 + half_width, y0 - half_height);
    LLTomas_drawLine(obj, x0 + half_width, y0 - half_height, x0 + half_width, y0 + half_height);
    LLTomas_drawLine(obj, x0 + half_width, y0 + half_height, x0 - half_width, y0 + half_height);
    LLTomas_drawLine(obj, x0 - half_width, y0 + half_height, x0 - half_width, y0 - half_height);
}


LLScanType_t LLTomas_virtualScan(LLTomas_t obj, float x0, float y0, float theta, float angle_increment, float angle_from, float angle_to)
{
    LLTomas_info_t* _obj = (LLTomas_info_t*)obj;

    size_t obj_count = SABuffer_count(&_obj->objs);

    LLScanType_t scan = LLScanType_create(_obj->timer);

    float angle = angle_from;
    angle_increment = LLTYPE_DEGREE_2_RAD(angle_increment);
    
    for(size_t angle_i = 0; angle < angle_to; angle_i++, angle += angle_increment)
    {
        float culc_angle = M_PI - angle;
        float min_hit = FLT_MAX;
        for(size_t obj_i = 0; obj_i < obj_count; obj_i++)
        {
            LLTomas_objLine_t* line_obj = (LLTomas_objLine_t*)SABuffer_readAtIndex_pointer(&_obj->objs, obj_i);

            float dir_x = cosf(culc_angle - theta) * -1;
            float dir_y = sinf(culc_angle - theta) * -1;

            float d_x = x0 - line_obj->x0;
            float d_y = y0 - line_obj->y0;

            float donom = line_obj->vec.x * dir_y - line_obj->vec.y * dir_x;
            if(10e-5 < fabs(donom))
            {
                float u = (d_x * dir_y - d_y * dir_x) / donom;
                if((0 <= u) & (u <= 1))
                {
                    float t = (line_obj->vec.x * d_y - line_obj->vec.y * d_x) / donom;
                    min_hit = ((0 < t) & (t < min_hit))? t : min_hit;
                }
            }
        }

        if(12 < min_hit)
        {
            min_hit = 0;
        }

        LLScanType_set(&scan, culc_angle, min_hit);
    }

    return scan;
}

#ifdef ENABLE_FPGA
static PYNQ_AXI_DMA g_dma;
static PYNQ_SHARED_MEMORY g_sms, g_smr;

void LLTomas_init(void)
{
    PYNQ_loadPartialBitstream("/tmp/design_1.bit");
    PYNQ_REGISTER reg;
    PYNQ_openRegister(&reg, 0xA0010000, 1);
    uint8_t start_byte = 0x81;
    PYNQ_setRegisterRange(&reg, 0x00, 0, 8, (unsigned int*)&start_byte);

    PYNQ_allocatedSharedMemory(&g_sms, sizeof(float)*7, 1);
    PYNQ_allocatedSharedMemory(&g_smr, sizeof(float)*1, 1);

    PYNQ_openDMA(&g_dma, 0xA0000000);
}

LLScanType_t LLTomas_simulateRange(LLTomas_t obj, float x0, float y0, float theta, LLScanType_t* scan)
{
    LLTomas_info_t* _obj = (LLTomas_info_t*)obj;

    LLScanType_t sim_scan = LLScanType_create(_obj->timer);

    size_t obj_count = SABuffer_count(&_obj->objs);

    size_t angle_count = LLScanType_count(scan);

    for(size_t angle_i = 0; angle_i < angle_count; angle_i++)
    {
        float angle = LLScanType_getRad(scan, angle_i);

        float min_hit = FLT_MAX;
        for(size_t obj_i = 0; obj_i < obj_count; obj_i++)
        {
            LLTomas_objLine_t* line_obj = (LLTomas_objLine_t*)SABuffer_readAtIndex_pointer(&_obj->objs, obj_i);
            float my_x = x0;
            float my_y = y0;
            float my_yaw = angle - theta;
            float target_x0 = line_obj->x0;
            float target_y0 = line_obj->y0;
            float target_vx = line_obj->vec.x;
            float target_vy = line_obj->vec.y;

            float* sms = (float*)g_sms.pointer;
            float* smr = (float*)g_smr.pointer;

            sms[0] = my_x;
            sms[1] = my_y;
            sms[2] = my_yaw;
            sms[3] = target_x0;
            sms[4] = target_y0;
            sms[5] = target_vx;
            sms[6] = target_vy;

            smr[0] = 0;

            PYNQ_writeDMA(&g_dma, &g_sms, 0, sizeof(float) * 7);
            PYNQ_readDMA(&g_dma, &g_smr, 0, sizeof(float) * 1);
            PYNQ_waitForDMAComplete(&g_dma, AXI_DMA_WRITE);

            PYNQ_waitForDMAComplete(&g_dma, AXI_DMA_READ);
            float hit = *(float*)g_smr.pointer;
            if(hit != 0)
            {
                min_hit = ((0 < hit) & (hit < min_hit))? hit : min_hit;
            }
        }


        if(12 < min_hit)
        {
            min_hit = 0;
        }

        LLScanType_set(&sim_scan, angle, min_hit);
    }
    return sim_scan;
}

#else /*ENABLE_FPGA*/
LLScanType_t LLTomas_simulateRange(LLTomas_t obj, float x0, float y0, float theta, LLScanType_t* scan)
{
    LLTomas_info_t* _obj = (LLTomas_info_t*)obj;

    LLScanType_t sim_scan = LLScanType_create(_obj->timer);

    size_t obj_count = SABuffer_count(&_obj->objs);

    size_t angle_count = LLScanType_count(scan);

    for(size_t angle_i = 0; angle_i < angle_count; angle_i++)
    {
        float angle = LLScanType_getRad(scan, angle_i);

        // float culc_angle = M_PI - angle;
        float min_hit = FLT_MAX;
        for(size_t obj_i = 0; obj_i < obj_count; obj_i++)
        {
            LLTomas_objLine_t* line_obj = (LLTomas_objLine_t*)SABuffer_readAtIndex_pointer(&_obj->objs, obj_i);

            float dir_x = cosf(angle - theta) * -1;
            float dir_y = sinf(angle - theta) * -1;

            float d_x = x0 - line_obj->x0;
            float d_y = y0 - line_obj->y0;

            float donom = line_obj->vec.x * dir_y - line_obj->vec.y * dir_x;
            if(10e-5 < fabs(donom))
            {
                float u = (d_x * dir_y - d_y * dir_x) / donom;
                if((0 <= u) & (u <= 1))
                {
                    float t = (line_obj->vec.x * d_y - line_obj->vec.y * d_x) / donom;
                    min_hit = ((0 < t) & (t < min_hit))? t : min_hit;
                }
            }
        }


        if(12 < min_hit)
        {
            min_hit = 0;
        }

        LLScanType_set(&sim_scan, angle, min_hit);
    }

    return sim_scan;
}
#endif /*ENABLE_FPGA*/
