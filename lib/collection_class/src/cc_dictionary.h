#ifndef CC_DICTIONARY_H
#define CC_DICTIONARY_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCDictionary_create(void);

size_t CCDictionary_count(CC_obj obj);
CC_obj CCDictionary_allKeys(CC_obj obj);
SABool_t CCDictionary_containsKey(CC_obj obj, const char* key);

CC_obj CCDictionary_allKeysForObject(CC_obj obj, CC_obj target_obj);

CC_obj CCDictionary_objectForKey(CC_obj obj, const char* key);
CC_obj CCDictionary_objectAtIndex(CC_obj obj, size_t index);

void CCDictionary_setObject(CC_obj obj, CC_obj set_obj, const char* key);

void CCDictionary_removeObjectForKey(CC_obj obj, const char* key);
void CCDictionary_removeObjectForObj(CC_obj obj, CC_obj target_obj);
void CCDictionary_removeAllObjects(CC_obj obj);

#ifdef __cplusplus
}
#endif

#endif /*CC_DICTIONARY_H*/
