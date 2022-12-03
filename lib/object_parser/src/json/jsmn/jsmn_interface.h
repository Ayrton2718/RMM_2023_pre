#ifndef JSON_JSMN_JSON_INTERFACE_H
#define JSON_JSMN_JSON_INTERFACE_H

#include "jsmn.h"

#include <std_api/std_api.h>

#ifdef __cplusplus
extern "C" {
#endif

struct JsmnInterface_token_t
{
    size_t index_start;
    size_t count;
};

struct JsmnInterface_t
{
    jsmn_parser             parser;

    char                   *js;
};

SABool_t JsmnInterface_parse(struct JsmnInterface_t* obj, const char* js, size_t js_len);
struct JsmnInterface_token_t JsmnInterface_getRoot(struct JsmnInterface_t* obj);

void JsmnInterface_destructor(struct JsmnInterface_t* obj);

void JsmnInterface_showResult(struct JsmnInterface_t* obj, struct JsmnInterface_token_t* token);

jsmntype_t JsmnInterface_jsonType(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token);

SABool_t JsmnInterface_dictionaryCount(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t* result);
SABool_t JsmnInterface_getDictionary(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, const char* key, struct JsmnInterface_token_t* result);
SABool_t JsmnInterface_getDictionaryAtIndex(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t dic_index, char* key, struct JsmnInterface_token_t* result);

SABool_t JsmnInterface_arrayCount(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t* result);
SABool_t JsmnInterface_getArray(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t index, struct JsmnInterface_token_t* result);

SABool_t JsmnInterface_parseAsInt(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, int* result);
SABool_t JsmnInterface_parseAsLong(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, long* result);
SABool_t JsmnInterface_parseAsDouble(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, double* result);
SABool_t JsmnInterface_parseAsBoolean(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, SABool_t* result);

SABool_t JsmnInterface_stringLen(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t* result);
SABool_t JsmnInterface_parseAsString(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, size_t copy_len, char* buff_out, size_t* len_out);
SABool_t JsmnInterface_getStringRaw(struct JsmnInterface_t* obj, const struct JsmnInterface_token_t* token, char** result);

#ifdef __cplusplus
}
#endif

#endif /*JSON_JSMN_JSON_INTERFACE_H*/