#ifndef CC_BINARY_H
#define CC_BINARY_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif


CC_obj CCBinary_createWithLength(const void* bin, size_t size);

size_t CCBinary_size(CC_obj obj);

void CCBinary_add(CC_obj obj, const void* bin, size_t size);

const uint8_t* CCBinary_getRaw(CC_obj obj);


#ifdef __cplusplus
}
#endif

#endif /*CC_BINARY_H*/
