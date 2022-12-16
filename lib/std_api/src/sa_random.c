#include "sa_random.h"

#include <stdlib.h>


SARandom_t SARandom_create(uint32_t seed)
{
    SARandom_t _obj;

    srand(seed);
    _obj.g_seed128_x = (uint32_t)rand();
    _obj.g_seed128_y = (uint32_t)rand();
    _obj.g_seed128_z = (uint32_t)rand();
    _obj.g_seed128_w = (uint32_t)rand();

    return _obj;
}


uint32_t SARandom_xorUint32(SARandom_t* obj)
{
    uint32_t t;
    t = obj->g_seed128_x ^ (obj->g_seed128_x << 11);
    obj->g_seed128_x = obj->g_seed128_y;
    obj->g_seed128_y = obj->g_seed128_z;
    obj->g_seed128_z = obj->g_seed128_w;
    obj->g_seed128_w = (obj->g_seed128_w ^ (obj->g_seed128_w >> 19)) ^ (t ^ (t >> 8));
    return obj->g_seed128_w;
}


int32_t SARandom_xorInt32Negaposi(SARandom_t* obj)
{
    uint32_t value = SARandom_xorUint32(obj);
    int32_t half = value >> 1;
    if(value % 2)
    {
        half *= -1;
    }
    return half;
}


float SARandom_xorFloat(SARandom_t* obj)
{
    return (float)((double)SARandom_xorUint32(obj) / (double)UINT32_MAX);
}

float SARandom_xorFloatNegaposi(SARandom_t* obj)
{
    uint32_t value = SARandom_xorUint32(obj);

    float data = (float)((double)SARandom_xorUint32(obj) / (double)UINT32_MAX);

    if(value % 2)
    {
        data *= -1;
    }
    
    return data;
}


double SARandom_xorDouble(SARandom_t* obj)
{
    return (double)SARandom_xorUint32(obj) / (double)UINT32_MAX;
}
