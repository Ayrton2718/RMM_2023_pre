#ifndef CC_ARRAY_H
#define CC_ARRAY_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCArray_create(void);

size_t CCArray_count(CC_obj obj);

SABool_t CCArray_containsObject(CC_obj obj, CC_obj target_obj);
size_t CCArray_indexOfObject(CC_obj obj, CC_obj target_obj);

CC_obj CCArray_objectAtIndex(CC_obj obj, size_t index);
CC_obj CCArray_lastObject(CC_obj obj);

void CCArray_addObject(CC_obj obj, CC_obj target_obj);
void CCArray_addObjectsFromArray(CC_obj obj, CC_obj target_array);
void CCArray_insertObject(CC_obj obj, size_t index, CC_obj target_obj);
void CCArray_replaceObjectAtIndex(CC_obj obj, size_t index, CC_obj target_obj);

void CCArray_removeLastObject(CC_obj obj);
void CCArray_removeObject(CC_obj obj, CC_obj target_obj);
void CCArray_removeObjectAtIndex(CC_obj obj, size_t index);
void CCArray_removeAllObject(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_ARRAY_H*/
