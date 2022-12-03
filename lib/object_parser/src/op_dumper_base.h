#ifndef OBJECT_PARSER_SRC_OP_DUMPER_BASE_H
#define OBJECT_PARSER_SRC_OP_DUMPER_BASE_H

#include <std_api/std_api.h>
#include "op_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef const uint8_t* (*OPDumper_dumpPtr_callback_t)(void* obj);
typedef size_t (*OPDumper_dumpLength_callback_t)(void* obj);

typedef void (*OPDumper_destructor_callback_t)(void* obj);

typedef void (*OPDumper_dictionary_start_callback_t)(void* obj, size_t count);
typedef void (*OPDumper_dictionary_end_callback_t)(void* obj);
typedef void (*OPDumper_dictionary_setKeyWithLength_callback_t)(void* obj, const char* key, size_t key_len);

typedef void (*OPDumper_array_start_callback_t)(void* obj, size_t count);
typedef void (*OPDumper_array_end_callback_t)(void* obj);

typedef void (*OPDumper_int_setSigned_callback_t)(void* obj, int64_t value);
typedef void (*OPDumper_int_setUnsigned_callback_t)(void* obj, uint64_t value);

typedef void (*OPDumper_real_setFloat_callback_t)(void* obj, float value);
typedef void (*OPDumper_real_setDouble_callback_t)(void* obj, double value);

typedef void (*OPDumper_bool_set_callback_t)(void* obj, SABool_t value);

typedef void (*OPDumper_string_setWithLength_callback_t)(void* obj, const char* string, size_t len);

typedef void (*OPDumper_binary_setWithLength_callback_t)(void* obj, const uint8_t* buff, size_t len);

typedef void (*OPDumper_null_callback_t)(void* obj);


struct OPDumper_base_t
{
    OPDumper_dumpPtr_callback_t     dumpPtr_callback;
    OPDumper_dumpLength_callback_t  dumpLength_callback;
    
    OPDumper_destructor_callback_t  destructor_callback;

    OPDumper_dictionary_start_callback_t                dictionary_start_callback;
    OPDumper_dictionary_end_callback_t                  dictionary_end_callback;
    OPDumper_dictionary_setKeyWithLength_callback_t     dictionary_setKeyWithLength_callback;

    OPDumper_array_start_callback_t     array_start_callback;
    OPDumper_array_end_callback_t       array_end_callback;

    OPDumper_int_setSigned_callback_t   int_setSigned_callback;
    OPDumper_int_setUnsigned_callback_t int_setUnsigned_callback;

    OPDumper_real_setFloat_callback_t   real_setFloat_callback;
    OPDumper_real_setDouble_callback_t  real_setDouble_callback;

    OPDumper_bool_set_callback_t    bool_set_callback;

    OPDumper_string_setWithLength_callback_t    string_setWithLength_callback;

    OPDumper_binary_setWithLength_callback_t   binary_setWithLength_callback;

    OPDumper_null_callback_t        null_callback;
};


#ifdef __cplusplus
}
#endif

#endif /*OBJECT_PARSER_SRC_OP_DUMPER_BASE_H*/