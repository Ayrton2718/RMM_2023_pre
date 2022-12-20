#ifndef CC_STRING_H
#define CC_STRING_H

#include "cc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

CC_obj CCString_create(const char* string);
CC_obj CCString_createWithLength(const char* string, size_t len);

size_t CCString_length(CC_obj obj);

const char* CCString_getCString(CC_obj obj);

void CCString_add(CC_obj obj, const char* string);

// default_value is a return value when parsing from string to number fails.
int CCString_intValue(CC_obj obj, int default_value);
float CCString_floatValue(CC_obj obj, float default_value);
double CCString_doubleValue(CC_obj obj, double default_value);
SABool_t CCString_boolValue(CC_obj obj, SABool_t default_value);

int CCString_compare(CC_obj obj, const char* compare_string);


#ifdef __cplusplus
}
#endif

#endif /*CC_STRING_H*/
