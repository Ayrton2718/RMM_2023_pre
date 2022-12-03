#include "op_dumper.h"

#include "op_dumper_base.h"


const uint8_t* OPDumper_dumpPtr(OPDumper_t obj)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    return _obj->dumpPtr_callback(obj);
}

size_t OPDumper_dumpLength(OPDumper_t obj)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    return _obj->dumpLength_callback(obj);
}

void OPDumper_dumpRead(OPDumper_t obj, uint8_t* out_buff, size_t out_len)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    size_t dump_len = _obj->dumpLength_callback(obj);
    if(out_len < dump_len)
    {
        dump_len = out_len;
        SALOG_ERROR("object_parser", "out_len is too small.");
    }

    memcpy(out_buff, _obj->dumpPtr_callback(obj), dump_len);
}


void OPDumper_destructor(OPDumper_t obj)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->destructor_callback(obj);
    SAMEM_FREE(obj);
}


void OPDumper_dictionary_start(OPDumper_t obj, size_t count)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->dictionary_start_callback(obj, count);
}

void OPDumper_dictionary_end(OPDumper_t obj)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->dictionary_end_callback(obj);
}

void OPDumper_dictionary_setKey(OPDumper_t obj, const char* key)
{
    OPDumper_dictionary_setKeyWithLength(obj, key, strlen(key));
}

void OPDumper_dictionary_setKeyWithLength(OPDumper_t obj, const char* key, size_t key_len)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->dictionary_setKeyWithLength_callback(obj, key, key_len);
}


void OPDumper_array_start(OPDumper_t obj, size_t count)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->array_start_callback(obj, count);
}

void OPDumper_array_end(OPDumper_t obj)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->array_end_callback(obj);
}


void OPDumper_int_setSigned(OPDumper_t obj, int64_t value)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->int_setSigned_callback(obj, value);
}

void OPDumper_int_setUnsigned(OPDumper_t obj, uint64_t value)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->int_setUnsigned_callback(obj, value);
}


void OPDumper_real_setFloat(OPDumper_t obj, float value)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->real_setFloat_callback(obj, value);
}

void OPDumper_real_setDouble(OPDumper_t obj, double value)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->real_setDouble_callback(obj, value);
}


void OPDumper_bool_set(OPDumper_t obj, SABool_t value)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->bool_set_callback(obj, value);
}


void OPDumper_string_set(OPDumper_t obj, const char* string)
{
    OPDumper_string_setWithLength(obj, string, strlen(string));
}

void OPDumper_string_setWithLength(OPDumper_t obj, const char* string, size_t len)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->string_setWithLength_callback(obj, string, len);
}


void OPDumper_binary_setWithLength(OPDumper_t obj, const uint8_t* buff, size_t len)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->binary_setWithLength_callback(obj, buff, len);
}

void OPDumper_null(OPDumper_t obj)
{
    struct OPDumper_base_t* _obj = (struct OPDumper_base_t*)obj;
    _obj->null_callback(obj);
}