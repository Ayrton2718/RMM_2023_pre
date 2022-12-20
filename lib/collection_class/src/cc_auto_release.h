#ifndef CC_AUTO_RELEASE_H
#define CC_AUTO_RELEASE_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void CCAutoRelease_ownerInit(void);

void CCAutoRelease_startScope(void);
void CCAutoRelease_doneScope(void);

CC_obj CCAutoRelease_add(CC_obj obj);

void __CCAutoRelease_addObjNolock(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_AUTO_RELEASE_H*/
