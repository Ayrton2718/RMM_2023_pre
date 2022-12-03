#include "op_msgpack_dumper.h"

#include "../op_dumper_base.h"
#include "msgpack-c/msgpack.h"

struct OPMsgpackDumper_info_t
{
    struct OPDumper_base_t base;

    msgpack_sbuffer sbuf;
    msgpack_packer packer;
};


const uint8_t* OPMsgpackDumper_dumpPtr_callback(void* obj);
size_t OPMsgpackDumper_dumpLength_callback(void* obj);

void OPMsgpackDumper_destructor_callback(void* obj);

void OPMsgpackDumper_dictionary_start_callback(void* obj, size_t count);
void OPMsgpackDumper_dictionary_end_callback(void* obj) {}
void OPMsgpackDumper_dictionary_setKeyWithLength_callback(void* obj, const char* key, size_t key_len);

void OPMsgpackDumper_array_start_callback(void* obj, size_t count);
void OPMsgpackDumper_array_end_callback(void* obj) {}

void OPMsgpackDumper_int_setSigned_callback(void* obj, int64_t value);
void OPMsgpackDumper_int_setUnsigned_callback(void* obj, uint64_t value);

void OPMsgpackDumper_real_setFloat_callback(void* obj, float value);
void OPMsgpackDumper_real_setDouble_callback(void* obj, double value);

void OPMsgpackDumper_bool_set_callback(void* obj, SABool_t value);

void OPMsgpackDumper_string_setWithLength_callback(void* obj, const char* string, size_t len);

void OPMsgpackDumper_binary_setWithLength_callback(void* obj, const uint8_t* buff, size_t len);

void OPMsgpackDumper_null_callback(void* obj);


OPDumper_t OPMsgpackDumper_create(void)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)SAMEM_MALLOC(sizeof(struct OPMsgpackDumper_info_t));
    
    msgpack_sbuffer_init(&_obj->sbuf);
    msgpack_packer_init(&_obj->packer, &_obj->sbuf, msgpack_sbuffer_write);

    _obj->base.dumpPtr_callback = OPMsgpackDumper_dumpPtr_callback;
    _obj->base.dumpLength_callback = OPMsgpackDumper_dumpLength_callback;

    _obj->base.destructor_callback = OPMsgpackDumper_destructor_callback;

    _obj->base.dictionary_start_callback = OPMsgpackDumper_dictionary_start_callback;
    _obj->base.dictionary_end_callback = OPMsgpackDumper_dictionary_end_callback;
    _obj->base.dictionary_setKeyWithLength_callback =  OPMsgpackDumper_dictionary_setKeyWithLength_callback;

    _obj->base.array_start_callback = OPMsgpackDumper_array_start_callback;
    _obj->base.array_end_callback = OPMsgpackDumper_array_end_callback;

    _obj->base.int_setSigned_callback = OPMsgpackDumper_int_setSigned_callback;
    _obj->base.int_setUnsigned_callback = OPMsgpackDumper_int_setUnsigned_callback;

    _obj->base.real_setFloat_callback = OPMsgpackDumper_real_setFloat_callback;
    _obj->base.real_setDouble_callback = OPMsgpackDumper_real_setDouble_callback;

    _obj->base.bool_set_callback = OPMsgpackDumper_bool_set_callback;

    _obj->base.string_setWithLength_callback = OPMsgpackDumper_string_setWithLength_callback;
    
    _obj->base.binary_setWithLength_callback = OPMsgpackDumper_binary_setWithLength_callback;

    _obj->base.null_callback = OPMsgpackDumper_null_callback;

    return _obj;
}


const uint8_t* OPMsgpackDumper_dumpPtr_callback(void* obj)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    return (const uint8_t*)_obj->sbuf.data;
}


size_t OPMsgpackDumper_dumpLength_callback(void* obj)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    return _obj->sbuf.size;
}


void OPMsgpackDumper_destructor_callback(void* obj)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    msgpack_sbuffer_destroy(&_obj->sbuf);
}


void OPMsgpackDumper_dictionary_start_callback(void* obj, size_t count)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    msgpack_pack_map(&_obj->packer, count);
}

void OPMsgpackDumper_dictionary_setKeyWithLength_callback(void* obj, const char* key, size_t key_len)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    msgpack_pack_str(&_obj->packer, key_len);
    msgpack_pack_str_body(&_obj->packer, key, key_len);
}


void OPMsgpackDumper_array_start_callback(void* obj, size_t count)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    msgpack_pack_array(&_obj->packer, count);
}


void OPMsgpackDumper_int_setSigned_callback(void* obj, int64_t value)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    if(value < 0)
    {
        if(INT16_MIN < value)
        {
            if(INT8_MIN < value)
            {
                msgpack_pack_int8(&_obj->packer, (int8_t)value);
            }else{
                msgpack_pack_int16(&_obj->packer, (int16_t)value);
            }
        }
        else if(INT32_MIN < value)
        {
            msgpack_pack_int32(&_obj->packer, (int32_t)value);
        }else{
            msgpack_pack_int64(&_obj->packer, (int64_t)value);
        }
    }
    else
    {
        if(value < INT16_MAX)
        {
            if(value < INT8_MAX)
            {
                msgpack_pack_int8(&_obj->packer, (int8_t)value);
            }else{
                msgpack_pack_int16(&_obj->packer, (int16_t)value);
            }
        }
        else if(value < INT32_MAX)
        {
            msgpack_pack_int32(&_obj->packer, (int32_t)value);
        }else{
            msgpack_pack_int64(&_obj->packer, (int64_t)value);
        }
    }
}

void OPMsgpackDumper_int_setUnsigned_callback(void* obj, uint64_t value)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    if(value < INT16_MAX)
    {
        if(value < INT8_MAX)
        {
            msgpack_pack_uint8(&_obj->packer, (int8_t)value);
        }else{
            msgpack_pack_uint16(&_obj->packer, (int16_t)value);
        }
    }
    else if(value < INT32_MAX)
    {
        msgpack_pack_uint32(&_obj->packer, (int32_t)value);
    }else{
        msgpack_pack_uint64(&_obj->packer, (int64_t)value);
    }
}


void OPMsgpackDumper_real_setFloat_callback(void* obj, float value)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    msgpack_pack_float(&_obj->packer, value);
}

void OPMsgpackDumper_real_setDouble_callback(void* obj, double value)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;  
    msgpack_pack_double(&_obj->packer, value); 
}


void OPMsgpackDumper_bool_set_callback(void* obj, SABool_t value)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    if(value)
    {
        msgpack_pack_true(&_obj->packer);
    }else{
        msgpack_pack_false(&_obj->packer);
    }
}


void OPMsgpackDumper_string_setWithLength_callback(void* obj, const char* string, size_t len)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    msgpack_pack_str(&_obj->packer, len);
    msgpack_pack_str_body(&_obj->packer, string, len);
}


void OPMsgpackDumper_binary_setWithLength_callback(void* obj, const uint8_t* buff, size_t len)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    msgpack_pack_bin(&_obj->packer, len);
    msgpack_pack_bin_body(&_obj->packer, buff, len);
}


void OPMsgpackDumper_null_callback(void* obj)
{
    struct OPMsgpackDumper_info_t* _obj = (struct OPMsgpackDumper_info_t*)obj;
    msgpack_pack_nil(&_obj->packer);
}