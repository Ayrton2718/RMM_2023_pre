#ifndef CC_INT_32_H
#define CC_INT_32_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCInt32_create(int32_t init_value);

void CCInt32_set(CC_obj obj, int32_t set_value);
int32_t CCInt32_get(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_INT_32_H*/
