#include "op_msgpack_loader.h"

#include "../op_loader_base.h"
#include "msgpack-c/msgpack.h"

struct OPMsgpackLoader_info_t
{
    struct OPLoader_base_t base;

    msgpack_object root_obj;
    msgpack_zone mempool;
};


SABool_t OPMsgpackLoader_load_callback(void* obj, const uint8_t* stream, size_t len);
void OPMsgpackLoader_destructor_callback(void* obj);

void OPMsgpackLoader_showResult_callback(void* obj);

OPType_token_t OPMsgpackLoader_getRoot_callback(void* obj);
OPType_objectType_t OPMsgpackLoader_getType_callback(void* obj, const OPType_token_t token);

SABool_t OPMsgpackLoader_dictionary_getCount_callback(void* obj, const OPType_token_t token, size_t* out);
SABool_t OPMsgpackLoader_dictionary_get_callback(void* obj, const OPType_token_t token, const char* key, OPType_token_t* out);
SABool_t OPMsgpackLoader_dictionary_getAtIndex_callback(void* obj, const OPType_token_t token, const size_t dic_index, char* key_out, OPType_token_t* out);

SABool_t OPMsgpackLoader_array_getCount_callback(void* obj, const OPType_token_t token, size_t* out);
SABool_t OPMsgpackLoader_array_get_callback(void* obj, const OPType_token_t token, const size_t ar_index, OPType_token_t* out);

SABool_t OPMsgpackLoader_int_getSigned_callback(void* obj, const OPType_token_t token, int64_t* out);
SABool_t OPMsgpackLoader_int_getUnsigned_callback(void* obj, const OPType_token_t token, uint64_t* out);

SABool_t OPMsgpackLoader_real_getFloat_callback(void* obj, const OPType_token_t token, float* out);
SABool_t OPMsgpackLoader_real_getDouble_callback(void* obj, const OPType_token_t token, double* out);

SABool_t OPMsgpackLoader_bool_get_callback(void* obj, const OPType_token_t token, SABool_t* out);

SABool_t OPMsgpackLoader_string_getLen_callback(void* obj, const OPType_token_t token, size_t* out);
SABool_t OPMsgpackLoader_string_get_callback(void* obj, const OPType_token_t token, const size_t copy_len, char* buff_out, size_t* len_out);
SABool_t OPMsgpackLoader_string_getRaw_callback(void* obj, const OPType_token_t token, char** out);

SABool_t OPMsgpackLoader_binary_getLen_callback(void* obj, const OPType_token_t token, size_t* out);
SABool_t OPMsgpackLoader_binary_get_callback(void* obj, const OPType_token_t token, const size_t copy_len, uint8_t* buff_out, size_t* len_out);
SABool_t OPMsgpackLoader_binary_getRaw_callback(void* obj, const OPType_token_t token, uint8_t** out);


OPLoader_t OPMsgpackLoader_create(void)
{
    struct OPMsgpackLoader_info_t* _obj = (struct OPMsgpackLoader_info_t*)SAMEM_MALLOC(sizeof(struct OPMsgpackLoader_info_t));
    
    msgpack_zone_init(&_obj->mempool, 4096);

    _obj->base.load_callback = OPMsgpackLoader_load_callback;
    _obj->base.destructor_callback = OPMsgpackLoader_destructor_callback;
    
    _obj->base.showResult_callback = OPMsgpackLoader_showResult_callback;

    _obj->base.getRoot_callback = OPMsgpackLoader_getRoot_callback;
    _obj->base.getType_callback = OPMsgpackLoader_getType_callback;

    _obj->base.dictionary_getCount_callback = OPMsgpackLoader_dictionary_getCount_callback;
    _obj->base.dictionary_get_callback = OPMsgpackLoader_dictionary_get_callback;
    _obj->base.dictionary_getAtIndex_callback = OPMsgpackLoader_dictionary_getAtIndex_callback;

    _obj->base.array_getCount_callback = OPMsgpackLoader_array_getCount_callback;
    _obj->base.array_get_callback = OPMsgpackLoader_array_get_callback;

    _obj->base.int_getSigned_callback = OPMsgpackLoader_int_getSigned_callback;
    _obj->base.int_getUnsigned_callback = OPMsgpackLoader_int_getUnsigned_callback;
    
    _obj->base.real_getFloat_callback = OPMsgpackLoader_real_getFloat_callback;
    _obj->base.real_getDouble_callback = OPMsgpackLoader_real_getDouble_callback;

    _obj->base.bool_get_callback = OPMsgpackLoader_bool_get_callback;

    _obj->base.string_getLen_callback = OPMsgpackLoader_string_getLen_callback;
    _obj->base.string_get_callback = OPMsgpackLoader_string_get_callback;
    _obj->base.string_getRaw_callback = OPMsgpackLoader_string_getRaw_callback;

    _obj->base.binary_getLen_callback = OPMsgpackLoader_binary_getLen_callback;
    _obj->base.binary_get_callback = OPMsgpackLoader_binary_get_callback;
    _obj->base.binary_getRaw_callback = OPMsgpackLoader_binary_getRaw_callback;

    return _obj;
}


SABool_t OPMsgpackLoader_load_callback(void* obj, const uint8_t* stream, size_t len)
{
    struct OPMsgpackLoader_info_t* _obj = (struct OPMsgpackLoader_info_t*)obj;

    msgpack_zone_clear(&_obj->mempool);

    msgpack_unpack_return res = msgpack_unpack((const char*)stream, len, NULL, &_obj->mempool, &_obj->root_obj);
    if(res != MSGPACK_UNPACK_SUCCESS)
    {
        SALOG_ERROR("object_parser", "MPack purse error (%d)", res);
        return SABOOL_FALSE;
    }
    return SABOOL_TRUE;
}

void OPMsgpackLoader_destructor_callback(void* obj)
{
    struct OPMsgpackLoader_info_t* _obj = (struct OPMsgpackLoader_info_t*)obj;
    msgpack_zone_destroy(&_obj->mempool);
}


void OPMsgpackLoader_showResult_callback(void* obj)
{
    struct OPMsgpackLoader_info_t* _obj = (struct OPMsgpackLoader_info_t*)obj;
    msgpack_object_print(stdout, _obj->root_obj);
}


OPType_token_t OPMsgpackLoader_getRoot_callback(void* obj)
{
    struct OPMsgpackLoader_info_t* _obj = (struct OPMsgpackLoader_info_t*)obj;
    return &_obj->root_obj;
}

OPType_objectType_t OPMsgpackLoader_getType_callback(void* obj, const OPType_token_t token)
{
    const msgpack_object* _de = (const msgpack_object*)token;

    OPType_objectType_t objType;

    switch (_de->type)
    {
    case MSGPACK_OBJECT_ARRAY:
        objType = OPType_objectType_ARRAY;
        break;

    case MSGPACK_OBJECT_MAP:
        objType = OPType_objectType_DICTIONARY;
        break;

    case MSGPACK_OBJECT_BOOLEAN:
        objType = OPType_objectType_BOOL;
        break;

    case MSGPACK_OBJECT_FLOAT:
    case MSGPACK_OBJECT_FLOAT32:
    // case MSGPACK_OBJECT_FLOAT64:
        objType = OPType_objectType_REAL;
        break;

    case MSGPACK_OBJECT_POSITIVE_INTEGER:
    case MSGPACK_OBJECT_NEGATIVE_INTEGER:
        objType = OPType_objectType_INT;
        break;

    case MSGPACK_OBJECT_BIN:
        objType = OPType_objectType_BIN;
        break;

    case MSGPACK_OBJECT_STR:
        objType = OPType_objectType_STR;
        break;

    case MSGPACK_OBJECT_NIL:
        objType = OPType_objectType_NULL;
        break;
    
    default:
        objType = OPType_objectType_NULL;
        break;
    }

    return objType;
}


SABool_t OPMsgpackLoader_dictionary_getCount_callback(void* obj, const OPType_token_t token, size_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;

    if(_de->type == MSGPACK_OBJECT_MAP)
    {
        *out = _de->via.map.size;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_dictionary_get_callback(void* obj, const OPType_token_t token, const char* key, OPType_token_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;

    if(_de->type == MSGPACK_OBJECT_MAP)
    {
        size_t count = _de->via.map.size;
        size_t key_len = strlen(key);
        SABool_t is_compair = SABOOL_FALSE;

        for(size_t i = 0; i < count; i++)
        {
            if(memcmp(_de->via.map.ptr[i].key.via.str.ptr, key, key_len) == 0)
            {
                *out = (OPType_token_t)&_de->via.map.ptr[i].val;
                is_compair = SABOOL_TRUE;
                break;
            }
        }
        return is_compair;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_dictionary_getAtIndex_callback(void* obj, const OPType_token_t token, const size_t dic_index, char* key_out, OPType_token_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_MAP)
    {
        if(dic_index < _de->via.map.size)
        {
            size_t key_len = _de->via.map.ptr[dic_index].key.via.str.size;
            if(128 <= key_len)
            {
                key_len = 128 - 1;
                SALOG_ERROR("object_parser", "Buffer overflowed.");
            }
            memcpy(key_out, _de->via.map.ptr[dic_index].key.via.str.ptr, key_len);
            key_out[key_len] = '\0';
            *out = (OPType_token_t)&_de->via.map.ptr[dic_index].val;
            return SABOOL_TRUE;
        }else{
            return SABOOL_FALSE;
        }
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t OPMsgpackLoader_array_getCount_callback(void* obj, const OPType_token_t token, size_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;

    if(_de->type == MSGPACK_OBJECT_ARRAY)
    {
        *out = _de->via.array.size;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_array_get_callback(void* obj, const OPType_token_t token, const size_t ar_index, OPType_token_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;

    if(_de->type == MSGPACK_OBJECT_ARRAY)
    {
        if(ar_index < _de->via.map.size)
        {
            *out = (OPType_token_t)&_de->via.array.ptr[ar_index];
            return SABOOL_TRUE;
        }else{
            return SABOOL_FALSE;
        }
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t OPMsgpackLoader_int_getSigned_callback(void* obj, const OPType_token_t token, int64_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_POSITIVE_INTEGER)
    {
        *out = (int64_t)_de->via.u64;
        return SABOOL_TRUE;
    }
    if(_de->type == MSGPACK_OBJECT_NEGATIVE_INTEGER)
    {
        *out = _de->via.i64;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_int_getUnsigned_callback(void* obj, const OPType_token_t token, uint64_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_POSITIVE_INTEGER)
    {
        *out = _de->via.u64;
        return SABOOL_TRUE;
    }
    if(_de->type == MSGPACK_OBJECT_NEGATIVE_INTEGER)
    {
        *out = (uint64_t)_de->via.i64;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t OPMsgpackLoader_real_getFloat_callback(void* obj, const OPType_token_t token, float* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_FLOAT32 || _de->type == MSGPACK_OBJECT_FLOAT64)
    {
        *out = (float)_de->via.f64;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_real_getDouble_callback(void* obj, const OPType_token_t token, double* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_FLOAT32 || _de->type == MSGPACK_OBJECT_FLOAT64)
    {
        *out = (double)_de->via.f64;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t OPMsgpackLoader_bool_get_callback(void* obj, const OPType_token_t token, SABool_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_BOOLEAN)
    {
        if(_de->via.boolean == 0)
        {
            *out = SABOOL_FALSE;
        }else{
            *out = SABOOL_TRUE;
        }
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t OPMsgpackLoader_string_getLen_callback(void* obj, const OPType_token_t token, size_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_STR)
    {
        *out = (size_t)_de->via.str.size;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_string_get_callback(void* obj, const OPType_token_t token, const size_t copy_len, char* buff_out, size_t* len_out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_STR)
    {
        size_t len = (size_t)_de->via.str.size;
        if(copy_len <= len)
        {
            len = copy_len - 1;
            SALOG_ERROR("object_parser", "Buffer overflowed.");
        }
        memcpy(buff_out, _de->via.str.ptr, len);
        buff_out[len] = '\0';
        *len_out = len;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_string_getRaw_callback(void* obj, const OPType_token_t token, char** out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_STR)
    {
        *out = (char*)_de->via.str.ptr;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


SABool_t OPMsgpackLoader_binary_getLen_callback(void* obj, const OPType_token_t token, size_t* out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_BIN)
    {
        *out = (size_t)_de->via.bin.size;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_binary_get_callback(void* obj, const OPType_token_t token, const size_t copy_len, uint8_t* buff_out, size_t* len_out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_BIN)
    {
        size_t len = (size_t)_de->via.bin.size;
        if(copy_len <= len)
        {
            len = copy_len;
            SALOG_ERROR("object_parser", "Buffer overflowed.");
        }
        memcpy(buff_out, _de->via.bin.ptr, len);
        *len_out = len;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

SABool_t OPMsgpackLoader_binary_getRaw_callback(void* obj, const OPType_token_t token, uint8_t** out)
{
    const msgpack_object* _de = (const msgpack_object*)token;
    if(_de->type == MSGPACK_OBJECT_STR)
    {
        *out = (uint8_t*)_de->via.bin.ptr;
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}