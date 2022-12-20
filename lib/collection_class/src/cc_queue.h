#ifndef CC_QUEUE_H
#define CC_QUEUE_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

// https://docs.oracle.com/javase/jp/8/docs/api/java/util/Queue.html

CC_obj CCQueue_create(void);

void CCQueue_add(CC_obj obj, CC_obj add_obj);

size_t CCQueue_count(CC_obj obj);

CC_obj CCQueue_peek(CC_obj obj);

void CCQueue_remove(CC_obj obj);

SABool_t CCQueue_readble(CC_obj obj);

void CCQueue_clear(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_QUEUE_H*/
