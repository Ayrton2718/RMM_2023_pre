#ifndef LRF_LOCALIZATION_SRC_LL_TYPE_H
#define LRF_LOCALIZATION_SRC_LL_TYPE_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include <std_api/std_api.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LLTYPE_DEGREE_2_RAD(degree) (degree * M_PI / 180)
#define LLTYPE_RAD_2_DEGREE(rad)    (rad * 180 / M_PI) 


typedef struct
{
    float x;
    float y;
    float yaw;
} LLType_pos_t;


#ifdef __cplusplus
}
#endif

#endif /*LRF_LOCALIZATION_SRC_LL_TYPE_H*/
