#ifndef OBJECT_PARSER_SRC_OP_TYPE_H
#define OBJECT_PARSER_SRC_OP_TYPE_H

#include <std_api/std_api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    OPType_objectType_NULL = 0,
    OPType_objectType_BOOL,
    OPType_objectType_INT,
    OPType_objectType_REAL,
    OPType_objectType_STR,
    OPType_objectType_ARRAY,
    OPType_objectType_DICTIONARY,
    OPType_objectType_BIN,
} OPType_objectType_t;

typedef void* OPType_token_t;

#ifdef __cplusplus
}
#endif

#endif /*OBJECT_PARSER_SRC_OP_TYPE_H*/
