#ifndef CC_TYPE_H
#define CC_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include <std_api/std_api.h>

#ifdef __APPLE__
#include <pthread.h>
#else
#include <pthread.h>
#include <sched.h>
#endif

// #define CCMEM_DEBUG_ON (1)

typedef void* CC_obj;

typedef uint32_t CC_type_t;
#define CC_OBJECT_ID            ((CC_type_t)(('C' << 24) | ('C' << 16) | ('O' << 8) | 'B'))

#define CC_UNKNOWN_TYPE_ID      ((CC_type_t)(('U' << 24) | ('N' << 16) | ('K' << 8) | 'N'))
#define CC_NULL_TYPE_ID         ((CC_type_t)(('N' << 24) | ('U' << 16) | ('L' << 8) | 'L'))
#define CC_BOOLEAN_TYPE_ID      ((CC_type_t)(('B' << 24) | ('O' << 16) | ('O' << 8) | 'L'))
#define CC_INT64_TYPE_ID        ((CC_type_t)(('I' << 24) | ('6' << 16) | ('4' << 8) | '_'))
#define CC_INT32_TYPE_ID        ((CC_type_t)(('I' << 24) | ('3' << 16) | ('2' << 8) | '_'))
#define CC_VARIABLE_TYPE_ID     ((CC_type_t)(('V' << 24) | ('A' << 16) | ('L' << 8) | 'I'))
#define CC_INT16_TYPE_ID        ((CC_type_t)(('I' << 24) | ('1' << 16) | ('6' << 8) | '_'))
#define CC_BYTES_TYPE_ID        ((CC_type_t)(('B' << 24) | ('Y' << 16) | ('T' << 8) | 'S'))
#define CC_FLOAT_TYPE_ID        ((CC_type_t)(('F' << 24) | ('3' << 16) | ('2' << 8) | '_'))
#define CC_DOUBLE_TYPE_ID       ((CC_type_t)(('F' << 24) | ('6' << 16) | ('4' << 8) | '_'))
#define CC_STRING_TYPE_ID       ((CC_type_t)(('S' << 24) | ('T' << 16) | ('R' << 8) | '_'))
#define CC_BINARY_TYPE_ID       ((CC_type_t)(('B' << 24) | ('I' << 16) | ('N' << 8) | '_'))
#define CC_STRUCT_TYPE_ID       ((CC_type_t)(('S' << 24) | ('T' << 16) | ('R' << 8) | 'C'))
#define CC_DICTIONARY_TYPE_ID   ((CC_type_t)(('D' << 24) | ('I' << 16) | ('C' << 8) | 'T'))
#define CC_ARRAY_TYPE_ID        ((CC_type_t)(('A' << 24) | ('R' << 16) | ('R' << 8) | 'A'))
#define CC_SET_TYPE_ID          ((CC_type_t)(('S' << 24) | ('E' << 16) | ('T' << 8) | '_'))
#define CC_QUEUE_TYPE_ID        ((CC_type_t)(('Q' << 24) | ('U' << 16) | ('E' << 8) | 'U'))

const char* CCType_toName(CC_type_t);

#ifdef __cplusplus
}
#endif

#endif /*CC_TYPE_H*/
