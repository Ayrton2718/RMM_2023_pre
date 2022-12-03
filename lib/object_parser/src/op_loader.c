#include "op_loader.h"

#include "op_loader_base.h"



SABool_t OPLoader_load(OPLoader_t obj, const uint8_t* stream, size_t len)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->load_callback(obj, stream, len);
}

void OPLoader_destructor(OPLoader_t obj)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    _obj->destructor_callback(obj);
    SAMEM_FREE(obj);
}


void OPLoader_showResult(OPLoader_t obj)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    _obj->showResult_callback(obj);
}


OPType_token_t OPLoader_getRoot(OPLoader_t obj)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->getRoot_callback(obj);
}

OPType_objectType_t OPLoader_getType(OPLoader_t obj, const OPType_token_t token)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->getType_callback(obj, token);
}


SABool_t OPLoader_dictionary_getCount(OPLoader_t obj, const OPType_token_t token, size_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->dictionary_getCount_callback(obj, token, out);
}

SABool_t OPLoader_dictionary_get(OPLoader_t obj, const OPType_token_t token, const char* key, OPType_token_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->dictionary_get_callback(obj, token, key, out);
}

SABool_t OPLoader_dictionary_getAtIndex(OPLoader_t obj, const OPType_token_t token, const size_t dic_index, char* key_out, OPType_token_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->dictionary_getAtIndex_callback(obj, token, dic_index, key_out, out);
}


SABool_t OPLoader_array_getCount(OPLoader_t obj, const OPType_token_t token, size_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->array_getCount_callback(obj, token, out);
}

SABool_t OPLoader_array_get(OPLoader_t obj, const OPType_token_t token, const size_t ar_index, OPType_token_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->array_get_callback(obj, token, ar_index, out);
}


SABool_t OPLoader_int_getSigned(OPLoader_t obj, const OPType_token_t token, int64_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->int_getSigned_callback(obj, token, out);
}

SABool_t OPLoader_int_getUnsigned(OPLoader_t obj, const OPType_token_t token, uint64_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->int_getUnsigned_callback(obj, token, out);
}


SABool_t OPLoader_real_getFloat(OPLoader_t obj, const OPType_token_t token, float* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->real_getFloat_callback(obj, token, out);
}

SABool_t OPLoader_real_getDouble(OPLoader_t obj, const OPType_token_t token, double* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->real_getDouble_callback(obj, token, out);
}


SABool_t OPLoader_bool_get(OPLoader_t obj, const OPType_token_t token, SABool_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->bool_get_callback(obj, token, out);
}


SABool_t OPLoader_string_getLen(OPLoader_t obj, const OPType_token_t token, size_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->string_getLen_callback(obj, token, out);
}

SABool_t OPLoader_string_get(OPLoader_t obj, const OPType_token_t token, const size_t copy_len, char* buff_out, size_t* len_out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->string_get_callback(obj, token, copy_len, buff_out, len_out);
}

SABool_t OPLoader_string_getRaw(OPLoader_t obj, const OPType_token_t token, char** out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->string_getRaw_callback(obj, token, out);
}


SABool_t OPLoader_binary_getLen(OPLoader_t obj, const OPType_token_t token, size_t* out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->binary_getLen_callback(obj, token, out);
}

SABool_t OPLoader_binary_get(OPLoader_t obj, const OPType_token_t token, const size_t copy_len, uint8_t* buff_out, size_t* len_out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->binary_get_callback(obj, token, copy_len, buff_out, len_out);
}

SABool_t OPLoader_binary_getRaw(OPLoader_t obj, const OPType_token_t token, uint8_t** out)
{
    struct OPLoader_base_t* _obj = (struct OPLoader_base_t*)obj;
    return _obj->binary_getRaw_callback(obj, token, out);
}
