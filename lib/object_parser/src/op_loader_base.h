#ifndef OBJECT_PARSER_SRC_OP_LOADER_BASE_H
#define OBJECT_PARSER_SRC_OP_LOADER_BASE_H

#include <std_api/std_api.h>
#include "op_type.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef SABool_t (*OPLoader_load_callback_t)(void* obj, const uint8_t* stream, size_t len);
typedef void (*OPLoader_destructor_callback_t)(void* obj);

typedef void (*OPLoader_showResult_callback_t)(void* obj);

typedef OPType_token_t (*OPLoader_getRoot_callback_t)(void* obj);
typedef OPType_objectType_t (*OPLoader_getType_callback_t)(void* obj, const OPType_token_t token);

typedef SABool_t (*OPLoader_dictionary_getCount_callback_t)(void* obj, const OPType_token_t token, size_t* out);
typedef SABool_t (*OPLoader_dictionary_get_callback_t)(void* obj, const OPType_token_t token, const char* key, OPType_token_t* out);
typedef SABool_t (*OPLoader_dictionary_getAtIndex_callback_t)(void* obj, const OPType_token_t token, const size_t dic_index, char* key_out, OPType_token_t* out);

typedef SABool_t (*OPLoader_array_getCount_callback_t)(void* obj, const OPType_token_t token, size_t* out);
typedef SABool_t (*OPLoader_array_get_callback_t)(void* obj, const OPType_token_t token, const size_t ar_index, OPType_token_t* out);

typedef SABool_t (*OPLoader_int_getSigned_callback_t)(void* obj, const OPType_token_t token, int64_t* out);
typedef SABool_t (*OPLoader_int_getUnsigned_callback_t)(void* obj, const OPType_token_t token, uint64_t* out);

typedef SABool_t (*OPLoader_real_getFloat_callback_t)(void* obj, const OPType_token_t token, float* out);
typedef SABool_t (*OPLoader_real_getDouble_callback_t)(void* obj, const OPType_token_t token, double* out);

typedef SABool_t (*OPLoader_bool_get_callback_t)(void* obj, const OPType_token_t token, SABool_t* out);

typedef SABool_t (*OPLoader_string_getLen_callback_t)(void* obj, const OPType_token_t token, size_t* out);
typedef SABool_t (*OPLoader_string_get_callback_t)(void* obj, const OPType_token_t token, const size_t copy_len, char* buff_out, size_t* len_out);
typedef SABool_t (*OPLoader_string_getRaw_callback_t)(void* obj, const OPType_token_t token, char** out);

typedef SABool_t (*OPLoader_binary_getLen_callback_t)(void* obj, const OPType_token_t token, size_t* out);
typedef SABool_t (*OPLoader_binary_get_callback_t)(void* obj, const OPType_token_t token, const size_t copy_len, uint8_t* buff_out, size_t* len_out);
typedef SABool_t (*OPLoader_binary_getRaw_callback_t)(void* obj, const OPType_token_t token, uint8_t** out);

struct OPLoader_base_t
{
    OPLoader_load_callback_t       load_callback;
    OPLoader_destructor_callback_t  destructor_callback;

    OPLoader_showResult_callback_t  showResult_callback;

    OPLoader_getRoot_callback_t     getRoot_callback;
    OPLoader_getType_callback_t     getType_callback;

    OPLoader_dictionary_getCount_callback_t     dictionary_getCount_callback;
    OPLoader_dictionary_get_callback_t          dictionary_get_callback;
    OPLoader_dictionary_getAtIndex_callback_t   dictionary_getAtIndex_callback;

    OPLoader_array_getCount_callback_t          array_getCount_callback;
    OPLoader_array_get_callback_t               array_get_callback;

    OPLoader_int_getSigned_callback_t           int_getSigned_callback;
    OPLoader_int_getUnsigned_callback_t       int_getUnsigned_callback;

    OPLoader_real_getFloat_callback_t           real_getFloat_callback;
    OPLoader_real_getDouble_callback_t          real_getDouble_callback;

    OPLoader_bool_get_callback_t                bool_get_callback;

    OPLoader_string_getLen_callback_t              string_getLen_callback;
    OPLoader_string_get_callback_t                 string_get_callback;
    OPLoader_string_getRaw_callback_t              string_getRaw_callback;

    OPLoader_binary_getLen_callback_t              binary_getLen_callback;
    OPLoader_binary_get_callback_t                 binary_get_callback;
    OPLoader_binary_getRaw_callback_t              binary_getRaw_callback;
};


#ifdef __cplusplus
}
#endif

#endif /*OBJECT_PARSER_SRC_OP_LOADER_BASE_H*/