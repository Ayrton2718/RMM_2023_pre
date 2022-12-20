#ifndef CC_STRUCT_H
#define CC_STRUCT_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif


CC_obj CCStruct_create(const void* bin, size_t size);

size_t CCStruct_size(CC_obj obj);

void CCStruct_set(CC_obj obj, const void* bin, size_t size);

void CCStruct_get(CC_obj obj, void* bin, size_t size);


#ifdef __cplusplus
}
#endif

#endif /*CC_STRUCT_H*/
