#include "op_json_dumper.h"

#include <std_api/std_api.h>
#include "../op_dumper_base.h"

struct OPJsonDumper_info_t
{
    struct OPDumper_base_t base;

    SABool_t is_readble;

    size_t cur_depth;

    SABuffer_t buffer;
};


const uint8_t* OPJsonDumper_dumpPtr_callback(void* obj);
size_t OPJsonDumper_dumpLength_callback(void* obj);

void OPJsonDumper_destructor_callback(void* obj);

void OPJsonDumper_dictionary_start_callback(void* obj, size_t count);
void OPJsonDumper_dictionary_end_callback(void* obj);
void OPJsonDumper_dictionary_setKeyWithLength_callback(void* obj, const char* key, size_t key_len);

void OPJsonDumper_array_start_callback(void* obj, size_t count);
void OPJsonDumper_array_end_callback(void* obj);

void OPJsonDumper_int_setSigned_callback(void* obj, int64_t value);
void OPJsonDumper_int_setUnsigned_callback(void* obj, uint64_t value);

void OPJsonDumper_real_setFloat_callback(void* obj, float value);
void OPJsonDumper_real_setDouble_callback(void* obj, double value);

void OPJsonDumper_bool_set_callback(void* obj, SABool_t value);

void OPJsonDumper_string_setWithLength_callback(void* obj, const char* string, size_t len);

void OPJsonDumper_binary_setWithLength_callback(void* obj, const uint8_t* buff, size_t len);

void OPJsonDumper_null_callback(void* obj);

static void OPJsonDumper_vars_termination(struct OPJsonDumper_info_t* _obj);


OPDumper_t OPJsonDumper_create(SABool_t is_readble)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)SAMEM_MALLOC(sizeof(struct OPJsonDumper_info_t));

    _obj->is_readble = is_readble;

    _obj->cur_depth = 0;

    SABuffer_create(&_obj->buffer, sizeof(char));
    
    _obj->base.dumpPtr_callback = OPJsonDumper_dumpPtr_callback;
    _obj->base.dumpLength_callback = OPJsonDumper_dumpLength_callback;

    _obj->base.destructor_callback = OPJsonDumper_destructor_callback;

    _obj->base.dictionary_start_callback = OPJsonDumper_dictionary_start_callback;
    _obj->base.dictionary_end_callback = OPJsonDumper_dictionary_end_callback;
    _obj->base.dictionary_setKeyWithLength_callback =  OPJsonDumper_dictionary_setKeyWithLength_callback;

    _obj->base.array_start_callback = OPJsonDumper_array_start_callback;
    _obj->base.array_end_callback = OPJsonDumper_array_end_callback;

    _obj->base.int_setSigned_callback = OPJsonDumper_int_setSigned_callback;
    _obj->base.int_setUnsigned_callback = OPJsonDumper_int_setUnsigned_callback;

    _obj->base.real_setFloat_callback = OPJsonDumper_real_setFloat_callback;
    _obj->base.real_setDouble_callback = OPJsonDumper_real_setDouble_callback;

    _obj->base.bool_set_callback = OPJsonDumper_bool_set_callback;

    _obj->base.string_setWithLength_callback = OPJsonDumper_string_setWithLength_callback;
    
    _obj->base.binary_setWithLength_callback = OPJsonDumper_binary_setWithLength_callback;

    _obj->base.null_callback = OPJsonDumper_null_callback;

    return _obj;
}


const uint8_t* OPJsonDumper_dumpPtr_callback(void* obj)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;
    char* tmp = SABuffer_readLast_pointer(&_obj->buffer);
    if(tmp == NULL)
    {
        SABuffer_add_stream(&_obj->buffer, sizeof("\0") - 1, "\0");
    }
    else if(*tmp != '\0')
    {
        SABuffer_add_stream(&_obj->buffer, sizeof("\0") - 1, "\0");
    }
    return SABuffer_getRaw(&_obj->buffer);
}

size_t OPJsonDumper_dumpLength_callback(void* obj)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;
    char* tmp = SABuffer_readLast_pointer(&_obj->buffer);
    if(tmp == NULL)
    {
        SABuffer_add_stream(&_obj->buffer, sizeof("\0") - 1, "\0");
    }
    else if(*tmp != '\0')
    {
        SABuffer_add_stream(&_obj->buffer, sizeof("\0") - 1, "\0");
    }
    return SABuffer_count(&_obj->buffer) - 1;
}


void OPJsonDumper_destructor_callback(void* obj)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;
    SABuffer_destructor(&_obj->buffer);
}


void OPJsonDumper_dictionary_start_callback(void* obj, size_t count)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    _obj->cur_depth++;
    if(_obj->is_readble)
    {
        SABuffer_add_stream(&_obj->buffer, sizeof("{\n") - 1, "{\n");
        for(size_t i = 0; i < _obj->cur_depth; i++)
        {
            SABuffer_add_stream(&_obj->buffer, sizeof("    ") - 1, "    ");
        }
        if(count == 0)
        {
            SABuffer_add(&_obj->buffer, " ");
        }
    }
    else
    {
        SABuffer_add(&_obj->buffer, "{");
        if(count == 0)
        {
            SABuffer_add(&_obj->buffer, " ");
        }
    }
}

void OPJsonDumper_dictionary_end_callback(void* obj)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    if(_obj->is_readble)
    {
        size_t remove_count = sizeof(",\n") - 1;
        remove_count += (sizeof("    ") - 1) * _obj->cur_depth;
        size_t remove_index = SABuffer_count(&_obj->buffer) - remove_count;
        SABuffer_remove_stream(&_obj->buffer, remove_index, remove_count);
        SABuffer_add_stream(&_obj->buffer, sizeof("\n") - 1, "\n");
        for(size_t i = 0; i < _obj->cur_depth - 1; i++)
        {
            SABuffer_add_stream(&_obj->buffer, sizeof("    ") - 1, "    ");
        }
        SABuffer_add_stream(&_obj->buffer, sizeof("}") - 1, "}");
    }
    else
    {
        SABuffer_replace(&_obj->buffer, SABuffer_count(&_obj->buffer) - 1, "}");
    }

    _obj->cur_depth--;

    OPJsonDumper_vars_termination(_obj);
}

void OPJsonDumper_dictionary_setKeyWithLength_callback(void* obj, const char* key, size_t key_len)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    SABuffer_add_stream(&_obj->buffer, sizeof("\"") - 1, "\"");
    SABuffer_add_stream(&_obj->buffer, key_len, key);

    if(_obj->is_readble)
    {
        SABuffer_add_stream(&_obj->buffer, sizeof("\": ") - 1, "\": ");
    }else{
        SABuffer_add_stream(&_obj->buffer, sizeof("\":") - 1, "\":");
    }
}


void OPJsonDumper_array_start_callback(void* obj, size_t count)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    _obj->cur_depth++;
    if(_obj->is_readble)
    {
        SABuffer_add_stream(&_obj->buffer, sizeof("[\n") - 1, "[\n");
        for(size_t i = 0; i < _obj->cur_depth; i++)
        {
            SABuffer_add_stream(&_obj->buffer, sizeof("    ") - 1, "    ");
        }
        if(count == 0)
        {
            SABuffer_add(&_obj->buffer, " ");
        }
    }
    else
    {
        SABuffer_add(&_obj->buffer, "[");
        if(count == 0)
        {
            SABuffer_add(&_obj->buffer, " ");
        }
    }
}

void OPJsonDumper_array_end_callback(void* obj)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    if(_obj->is_readble)
    {
        size_t remove_count = sizeof(",\n") - 1;
        remove_count += (sizeof("    ") - 1) * _obj->cur_depth;
        size_t remove_index = SABuffer_count(&_obj->buffer) - remove_count;
        SABuffer_remove_stream(&_obj->buffer, remove_index, remove_count);
        SABuffer_add_stream(&_obj->buffer, sizeof("\n") - 1, "\n");
        for(size_t i = 0; i < _obj->cur_depth - 1; i++)
        {
            SABuffer_add_stream(&_obj->buffer, sizeof("    ") - 1, "    ");
        }
        SABuffer_add_stream(&_obj->buffer, sizeof("]") - 1, "]");
    }
    else
    {
        SABuffer_replace(&_obj->buffer, SABuffer_count(&_obj->buffer) - 1, "]");
    }

    _obj->cur_depth--;

    OPJsonDumper_vars_termination(_obj);
}


void OPJsonDumper_int_setSigned_callback(void* obj, int64_t value)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    size_t len;
    char buff[128];
    len = sprintf(buff, "%lld", value);

    if(0 < len)
    {
        SABuffer_add_stream(&_obj->buffer, len, buff);
    }else{
        SABuffer_add_stream(&_obj->buffer, sizeof("null") - 1, "null");
        SALOG_ERROR("object_parser", "sprintf return 0");
    }

    OPJsonDumper_vars_termination(_obj);
}

void OPJsonDumper_int_setUnsigned_callback(void* obj, uint64_t value)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    size_t len;
    char buff[128];
    len = sprintf(buff, "%lld", value);

    if(0 < len)
    {
        SABuffer_add_stream(&_obj->buffer, len, buff);
    }else{
        SABuffer_add_stream(&_obj->buffer, sizeof("null") - 1, "null");
        SALOG_ERROR("object_parser", "sprintf return 0");
    }

    OPJsonDumper_vars_termination(_obj);
}


void OPJsonDumper_real_setFloat_callback(void* obj, float value)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    size_t len;
    char buff[128];
    len = sprintf(buff, "%f", value);

    if(0 < len)
    {
        SABuffer_add_stream(&_obj->buffer, len, buff);
    }else{
        SABuffer_add_stream(&_obj->buffer, sizeof("null") - 1, "null");
        SALOG_ERROR("object_parser", "sprintf return 0");
    }

    OPJsonDumper_vars_termination(_obj);
}

void OPJsonDumper_real_setDouble_callback(void* obj, double value)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    size_t len;
    char buff[128];
    len = sprintf(buff, "%lf", value);

    if(0 < len)
    {
        SABuffer_add_stream(&_obj->buffer, len, buff);
    }else{
        SABuffer_add_stream(&_obj->buffer, sizeof("null") - 1, "null");
        SALOG_ERROR("object_parser", "sprintf return 0");
    }

    OPJsonDumper_vars_termination(_obj);
}


void OPJsonDumper_bool_set_callback(void* obj, SABool_t value)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    if(value)
    {
        SABuffer_add_stream(&_obj->buffer, sizeof("true") - 1, "true");
    }else{
        SABuffer_add_stream(&_obj->buffer, sizeof("false") - 1, "false");
    }

    OPJsonDumper_vars_termination(_obj);
}


void OPJsonDumper_string_setWithLength_callback(void* obj, const char* string, size_t len)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    SABuffer_add(&_obj->buffer, "\"");
    SABuffer_add_stream(&_obj->buffer, len, string);
    SABuffer_add(&_obj->buffer, "\"");

    OPJsonDumper_vars_termination(_obj);
}


void OPJsonDumper_binary_setWithLength_callback(void* obj, const uint8_t* buff, size_t len)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;

    SABuffer_add_stream(&_obj->buffer, sizeof("binary") - 1, "binary");
    SALOG_ERROR("object_parser", "Can't dump a binary!");

    OPJsonDumper_vars_termination(_obj);
}


void OPJsonDumper_null_callback(void* obj)
{
    struct OPJsonDumper_info_t* _obj = (struct OPJsonDumper_info_t*)obj;
    SABuffer_add_stream(&_obj->buffer, sizeof("null") - 1, "null");
    OPJsonDumper_vars_termination(_obj);
}


static void OPJsonDumper_vars_termination(struct OPJsonDumper_info_t* _obj)
{
    if(_obj->cur_depth != 0)
    {
        if(_obj->is_readble)
        {
            SABuffer_add_stream(&_obj->buffer, sizeof(",\n") - 1, ",\n");
            for(size_t i = 0; i < _obj->cur_depth; i++)
            {
                SABuffer_add_stream(&_obj->buffer, sizeof("    ") - 1, "    ");
            }
        }
        else
        {
            SABuffer_add_stream(&_obj->buffer, sizeof(",") - 1, ",");
        }
    }
}