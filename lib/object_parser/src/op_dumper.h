#ifndef OBJECT_PARSER_SRC_OP_DUMPER_H
#define OBJECT_PARSER_SRC_OP_DUMPER_H

#include <std_api/std_api.h>

#include "op_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* OPDumper_t;

const uint8_t* OPDumper_dumpPtr(OPDumper_t obj);
size_t OPDumper_dumpLength(OPDumper_t obj);
void OPDumper_dumpRead(OPDumper_t obj, uint8_t* out_buff, size_t out_len);

void OPDumper_destructor(OPDumper_t obj);

void OPDumper_dictionary_start(OPDumper_t obj, size_t count);
void OPDumper_dictionary_end(OPDumper_t obj);
void OPDumper_dictionary_setKey(OPDumper_t obj, const char* key);
void OPDumper_dictionary_setKeyWithLength(OPDumper_t obj, const char* key, size_t key_len);

void OPDumper_array_start(OPDumper_t obj, size_t count);
void OPDumper_array_end(OPDumper_t obj);

void OPDumper_int_setSigned(OPDumper_t obj, int64_t value);
void OPDumper_int_setUnsigned(OPDumper_t obj, uint64_t value);

void OPDumper_real_setFloat(OPDumper_t obj, float value);
void OPDumper_real_setDouble(OPDumper_t obj, double value);

void OPDumper_bool_set(OPDumper_t obj, SABool_t value);

void OPDumper_string_set(OPDumper_t obj, const char* string);
void OPDumper_string_setWithLength(OPDumper_t obj, const char* string, size_t len);

void OPDumper_binary_setWithLength(OPDumper_t obj, const uint8_t* buff, size_t len);

void OPDumper_null(OPDumper_t obj);

#ifdef __cplusplus
}
#endif

#endif /*OBJECT_PARSER_SRC_OP_DUMPER_H*/
