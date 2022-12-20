#ifndef CC_BOOL_H
#define CC_BOOL_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCBool_create(SABool_t init_value);

void CCBool_set(CC_obj obj, SABool_t set_value);

SABool_t CCBool_get(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_BOOL_H*/
