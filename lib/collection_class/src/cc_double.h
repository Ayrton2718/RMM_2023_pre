#ifndef CC_DOUBLE_H
#define CC_DOUBLE_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCDouble_create(double init_value);

void CCDouble_set(CC_obj obj, double set_value);

double CCDouble_get(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_DOUBLE_H*/
