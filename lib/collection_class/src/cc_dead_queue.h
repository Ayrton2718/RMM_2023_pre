#ifndef CC_DEAD_QUEUE_H
#define CC_DEAD_QUEUE_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void CCDeadQueue_initializer(void);

void CCDeadQueue_add(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_DEAD_QUEUE_H*/
