#ifndef STD_API_SRC_SA_RANDOM_H
#define STD_API_SRC_SA_RANDOM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint32_t g_seed128_x;
    uint32_t g_seed128_y;
    uint32_t g_seed128_z;
    uint32_t g_seed128_w;
} SARandom_t;


SARandom_t SARandom_create(uint32_t seed);

uint32_t SARandom_xorUint32(SARandom_t* obj);
int32_t SARandom_xorInt32Negaposi(SARandom_t* obj);

float SARandom_xorFloat(SARandom_t* obj);
float SARandom_xorFloatNegaposi(SARandom_t* obj);

double SARandom_xorDouble(SARandom_t* obj);


#ifdef __cplusplus
}
#endif

#endif /*STD_API_SRC_SA_RANDOM_H*/