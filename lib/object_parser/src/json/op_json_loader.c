#include "op_json_loader.h"

#include "../op_loader_base.h"
#include "jsmn/jsmn_interface.h"
#include <std_api/std_api.h>


struct OPJsonLoader_info_t
{
    struct OPLoader_base_t base;

    struct JsmnInterface_t jsmn_if;
    SABuffer_t token_buff;
};


SABool_t OPJsonLoader_load_callback(void* obj, const uint8_t* stream, size_t len);
void OPJsonLoader_destructor_callback(void* obj);

void OPJsonLoader_showResult_callback(void* obj);

OPType_token_t OPJsonLoader_getRoot_callback(void* obj);
OPType_objectType_t OPJsonLoader_getType_callback(void* obj, const OPType_token_t token);

SABool_t OPJsonLoader_dictionary_getCount_callback(void* obj, const OPType_token_t token, size_t* out);
SABool_t OPJsonLoader_dictionary_get_callback(void* obj, const OPType_token_t token, const char* key, OPType_token_t* out);
SABool_t OPJsonLoader_dictionary_getAtIndex_callback(void* obj, const OPType_token_t token, const size_t dic_index, char* key_out, OPType_token_t* out);

SABool_t OPJsonLoader_array_getCount_callback(void* obj, const OPType_token_t token, size_t* out);
SABool_t OPJsonLoader_array_get_callback(void* obj, const OPType_token_t token, const size_t ar_index, OPType_token_t* out);

SABool_t OPJsonLoader_int_getSigned_callback(void* obj, const OPType_token_t token, int64_t* out);
SABool_t OPJsonLoader_int_getUnsigned_callback(void* obj, const OPType_token_t token, uint64_t* out);

SABool_t OPJsonLoader_real_getFloat_callback(void* obj, const OPType_token_t token, float* out);
SABool_t OPJsonLoader_real_getDouble_callback(void* obj, const OPType_token_t token, double* out);

SABool_t OPJsonLoader_bool_get_callback(void* obj, const OPType_token_t token, SABool_t* out);

SABool_t OPJsonLoader_string_getLen_callback(void* obj, const OPType_token_t token, size_t* out);
SABool_t OPJsonLoader_string_get_callback(void* obj, const OPType_token_t token, const size_t copy_len, char* buff_out, size_t* len_out);
SABool_t OPJsonLoader_string_getRaw_callback(void* obj, const OPType_token_t token, char** out);

SABool_t OPJsonLoader_binary_getLen_callback(void* obj, const OPType_token_t token, size_t* out);
SABool_t OPJsonLoader_binary_get_callback(void* obj, const OPType_token_t token, const size_t copy_len, uint8_t* buff_out, size_t* len_out);
SABool_t OPJsonLoader_binary_getRaw_callback(void* obj, const OPType_token_t token, uint8_t** out);


static OPType_token_t OPJsonLoader_getTokenRange(struct OPJsonLoader_info_t* _obj, size_t token_index);


OPLoader_t OPJsonLoader_create(void)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)SAMEM_MALLOC(sizeof(struct OPJsonLoader_info_t));

    SABuffer_create(&_obj->token_buff, sizeof(struct JsmnInterface_token_t));
    
    _obj->base.load_callback = OPJsonLoader_load_callback;
    _obj->base.destructor_callback = OPJsonLoader_destructor_callback;
    
    _obj->base.showResult_callback = OPJsonLoader_showResult_callback;

    _obj->base.getRoot_callback = OPJsonLoader_getRoot_callback;
    _obj->base.getType_callback = OPJsonLoader_getType_callback;

    _obj->base.dictionary_getCount_callback = OPJsonLoader_dictionary_getCount_callback;
    _obj->base.dictionary_get_callback = OPJsonLoader_dictionary_get_callback;
    _obj->base.dictionary_getAtIndex_callback = OPJsonLoader_dictionary_getAtIndex_callback;

    _obj->base.array_getCount_callback = OPJsonLoader_array_getCount_callback;
    _obj->base.array_get_callback = OPJsonLoader_array_get_callback;

    _obj->base.int_getSigned_callback = OPJsonLoader_int_getSigned_callback;
    _obj->base.int_getUnsigned_callback = OPJsonLoader_int_getUnsigned_callback;
    
    _obj->base.real_getFloat_callback = OPJsonLoader_real_getFloat_callback;
    _obj->base.real_getDouble_callback = OPJsonLoader_real_getDouble_callback;

    _obj->base.bool_get_callback = OPJsonLoader_bool_get_callback;

    _obj->base.string_getLen_callback = OPJsonLoader_string_getLen_callback;
    _obj->base.string_get_callback = OPJsonLoader_string_get_callback;
    _obj->base.string_getRaw_callback = OPJsonLoader_string_getRaw_callback;

    // _obj->base.binary_getLen_callback = OPJsonLoader_binary_getLen_callback;
    // _obj->base.binary_get_callback = OPJsonLoader_binary_get_callback;
    // _obj->base.binary_getRaw_callback = OPJsonLoader_binary_getRaw_callback;

    return _obj;
}

SABool_t OPJsonLoader_load_callback(void* obj, const uint8_t* stream, size_t len)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABuffer_clear(&_obj->token_buff);
    return JsmnInterface_parse(&_obj->jsmn_if, (const char*)stream, len);
}

void OPJsonLoader_destructor_callback(void* obj)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;
    SABuffer_destructor(&_obj->token_buff);
    JsmnInterface_destructor(&_obj->jsmn_if);
}


void OPJsonLoader_showResult_callback(void* obj)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;
    JsmnInterface_showResult(&_obj->jsmn_if, OPJsonLoader_getRoot_callback(obj));
}


OPType_token_t OPJsonLoader_getRoot_callback(void* obj)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    struct JsmnInterface_token_t token = JsmnInterface_getRoot(&_obj->jsmn_if);
    SABuffer_add(&_obj->token_buff, &token);
    return (OPType_token_t)SABuffer_readLast_pointer(&_obj->token_buff);
}

OPType_objectType_t OPJsonLoader_getType_callback(void* obj, const OPType_token_t token)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    OPType_objectType_t objType;

    switch(JsmnInterface_jsonType(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token))
    {
    case JSMN_ARRAY:
        objType = OPType_objectType_ARRAY;
        break;

    case JSMN_OBJECT:
        objType = OPType_objectType_DICTIONARY;
        break;

    case JSMN_TRUE:
    case JSMN_FALSE:
        objType = OPType_objectType_BOOL;
        break;

    case JSMN_DOUBLE:
        objType = OPType_objectType_REAL;
        break;

    case JSMN_INT:
        objType = OPType_objectType_INT;
        break;

    case JSMN_STRING:
        objType = OPType_objectType_STR;
        break;

    // case JSMN_NULL:
    //     objType = OPType_objectType_NULL;
    //     break;

    case JSMN_UNDEFINED:
        objType = OPType_objectType_NULL;
    
    default:
        objType = OPType_objectType_NULL;
        break;
    }

    return objType;
}


SABool_t OPJsonLoader_dictionary_getCount_callback(void* obj, const OPType_token_t token, size_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_DICTIONARY)
    {
        is_success = JsmnInterface_dictionaryCount(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, out);
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}

SABool_t OPJsonLoader_dictionary_get_callback(void* obj, const OPType_token_t token, const char* key, OPType_token_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;
    struct JsmnInterface_token_t out_token;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_DICTIONARY)
    {
        if(JsmnInterface_getDictionary(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, key, &out_token))
        {
            SABuffer_add(&_obj->token_buff, &out_token);
            *out = (OPType_token_t)SABuffer_readLast_pointer(&_obj->token_buff);
            is_success = SABOOL_TRUE;
        }else{
            is_success = SABOOL_FALSE;
        }
    }else{
        is_success = SABOOL_FALSE;
    }

    return is_success;
}

SABool_t OPJsonLoader_dictionary_getAtIndex_callback(void* obj, const OPType_token_t token, const size_t dic_index, char* key_out, OPType_token_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;
    struct JsmnInterface_token_t out_token;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_DICTIONARY)
    {
        if(JsmnInterface_getDictionaryAtIndex(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, dic_index, key_out, &out_token))
        {
            SABuffer_add(&_obj->token_buff, &out_token);
            *out = (OPType_token_t)SABuffer_readLast_pointer(&_obj->token_buff);
            is_success = SABOOL_TRUE;
        }else{
            is_success = SABOOL_FALSE;
        }
    }else{
        is_success = SABOOL_FALSE;
    }

    return is_success;
}


SABool_t OPJsonLoader_array_getCount_callback(void* obj, const OPType_token_t token, size_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_ARRAY)
    {
        is_success = JsmnInterface_arrayCount(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, out);
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}

SABool_t OPJsonLoader_array_get_callback(void* obj, const OPType_token_t token, const size_t ar_index, OPType_token_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;
    struct JsmnInterface_token_t out_token;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_ARRAY)
    {
        if(JsmnInterface_getArray(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, ar_index, &out_token))
        {
            SABuffer_add(&_obj->token_buff, &out_token);
            *out = (OPType_token_t)SABuffer_readLast_pointer(&_obj->token_buff);
            is_success = SABOOL_TRUE;
        }else{
            is_success = SABOOL_FALSE;
        }
    }else{
        is_success = SABOOL_FALSE;
    }

    return is_success;
}


SABool_t OPJsonLoader_int_getSigned_callback(void* obj, const OPType_token_t token, int64_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_INT)
    {
        long value = (long)*out;
        is_success = JsmnInterface_parseAsLong(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, &value);
        *out = (int64_t)value;
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}

SABool_t OPJsonLoader_int_getUnsigned_callback(void* obj, const OPType_token_t token, uint64_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_INT)
    {
        long value = (long)*out;
        is_success = JsmnInterface_parseAsLong(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, &value);
        *out = (int64_t)value;
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}


SABool_t OPJsonLoader_real_getFloat_callback(void* obj, const OPType_token_t token, float* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;
    
    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_REAL)
    {
        double value = (double)*out;
        is_success = JsmnInterface_parseAsDouble(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, &value);
        *out = (float)value;
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}

SABool_t OPJsonLoader_real_getDouble_callback(void* obj, const OPType_token_t token, double* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_REAL)
    {
        double value = (double)*out;
        is_success = JsmnInterface_parseAsDouble(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, &value);
        *out = (double)value;
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}

SABool_t OPJsonLoader_bool_get_callback(void* obj, const OPType_token_t token, SABool_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_BOOL)
    {
        SABool_t value = *out;
        is_success = JsmnInterface_parseAsBoolean(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, &value);
        *out = (SABool_t)value;
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}


SABool_t OPJsonLoader_string_getLen_callback(void* obj, const OPType_token_t token, size_t* out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_STR)
    {
        is_success = JsmnInterface_stringLen(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, out);
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}

SABool_t OPJsonLoader_string_get_callback(void* obj, const OPType_token_t token, const size_t copy_len, char* buff_out, size_t* len_out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj; 

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_STR)
    {
        is_success = JsmnInterface_parseAsString(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, copy_len, buff_out, len_out);
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}

SABool_t OPJsonLoader_string_getRaw_callback(void* obj, const OPType_token_t token, char** out)
{
    struct OPJsonLoader_info_t* _obj = (struct OPJsonLoader_info_t*)obj;

    SABool_t is_success = SABOOL_TRUE;
    if(OPJsonLoader_getType_callback(obj, token) == OPType_objectType_STR)
    {
        is_success = JsmnInterface_getStringRaw(&_obj->jsmn_if, (struct JsmnInterface_token_t*)token, out);
    }else{
        is_success = SABOOL_FALSE;
    }
    return is_success;
}
