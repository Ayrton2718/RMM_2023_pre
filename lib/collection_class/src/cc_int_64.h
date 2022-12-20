#ifndef CC_INT_64_H
#define CC_INT_64_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCInt64_create(int64_t init_value);

void CCInt64_set(CC_obj obj, int64_t set_value);
int64_t CCInt64_get(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_INT_64_H*/
