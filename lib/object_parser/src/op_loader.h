#ifndef OBJECT_PARSER_SRC_OP_LOADER_H
#define OBJECT_PARSER_SRC_OP_LOADER_H

#include <std_api/std_api.h>
#include "op_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* OPLoader_t;


SABool_t OPLoader_load(OPLoader_t obj, const uint8_t* stream, size_t len);
void OPLoader_destructor(OPLoader_t obj);

void OPLoader_showResult(OPLoader_t obj);

OPType_token_t OPLoader_getRoot(OPLoader_t obj);
OPType_objectType_t OPLoader_getType(OPLoader_t obj, const OPType_token_t token);

SABool_t OPLoader_dictionary_getCount(OPLoader_t obj, const OPType_token_t token, size_t* out);
SABool_t OPLoader_dictionary_get(OPLoader_t obj, const OPType_token_t token, const char* key, OPType_token_t* out);
SABool_t OPLoader_dictionary_getAtIndex(OPLoader_t obj, const OPType_token_t token, const size_t dic_index, char* key_out, OPType_token_t* out);

SABool_t OPLoader_array_getCount(OPLoader_t obj, const OPType_token_t token, size_t* out);
SABool_t OPLoader_array_get(OPLoader_t obj, const OPType_token_t token, const size_t ar_index, OPType_token_t* out);

SABool_t OPLoader_int_getSigned(OPLoader_t obj, const OPType_token_t token, int64_t* out);
SABool_t OPLoader_int_getUnsigned(OPLoader_t obj, const OPType_token_t token, uint64_t* out);

SABool_t OPLoader_real_getFloat(OPLoader_t obj, const OPType_token_t token, float* out);
SABool_t OPLoader_real_getDouble(OPLoader_t obj, const OPType_token_t token, double* out);

SABool_t OPLoader_bool_get(OPLoader_t obj, const OPType_token_t token, SABool_t* out);

SABool_t OPLoader_string_getLen(OPLoader_t obj, const OPType_token_t token, size_t* out);
SABool_t OPLoader_string_get(OPLoader_t obj, const OPType_token_t token, const size_t copy_len, char* buff_out, size_t* len_out);
SABool_t OPLoader_string_getRaw(OPLoader_t obj, const OPType_token_t token, char** out);

SABool_t OPLoader_binary_getLen(OPLoader_t obj, const OPType_token_t token, size_t* out);
SABool_t OPLoader_binary_get(OPLoader_t obj, const OPType_token_t token, const size_t copy_len, uint8_t* buff_out, size_t* len_out);
SABool_t OPLoader_binary_getRaw(OPLoader_t obj, const OPType_token_t token, uint8_t** out);

#ifdef __cplusplus
}
#endif

#endif /*OBJECT_PARSER_SRC_OP_LOADER_H*/
