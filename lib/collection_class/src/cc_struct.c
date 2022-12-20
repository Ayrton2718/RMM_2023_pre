#include "cc_struct.h"

#include "cc_raw_binary.h"


static void CCStruct_set_nolock(CC_obj obj, const void* bin, size_t size);
static void CCStruct_get_nolock(CC_obj obj, void* bin, size_t size);

void CCStruct_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
SABool_t CCStruct_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


CC_obj CCStruct_create(const void* bin, size_t size)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)CCRawBinary_create(size, CC_STRUCT_TYPE_ID, CCStruct_dump_callback_nolock, CCStruct_load_callback_nolock);
    if(bin != NULL)
    {
        memcpy(_obj->data_bin, bin, _obj->data_size);
    }else{
        memset(_obj->data_bin, 0x00, _obj->data_size);
    }
    return _obj;
}

void CCStruct_set(CC_obj obj, const void* bin, size_t size)
{
    if(CCBaseObject_isTypeObject(obj, CC_STRUCT_TYPE_ID))
    {
        struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
        if(_obj->data_size == size)
        {
            CCBaseObject_lock(obj);
            memcpy(_obj->data_bin, bin, _obj->data_size);
            CCBaseObject_unlock(obj);
        }else{
            SALOG_ERROR("collection_class", "Wrong size %d->%d", size, _obj->data_size);
        }
    }
}

void CCStruct_get(CC_obj obj, void* bin, size_t size)
{
    if(CCBaseObject_isTypeObject(obj, CC_STRUCT_TYPE_ID))
    {
        struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
        if(_obj->data_size == size)
        {
            CCBaseObject_lock(obj);
            memcpy(bin, _obj->data_bin, _obj->data_size);
            CCBaseObject_unlock(obj);
        }else{
            SALOG_ERROR("collection_class", "Wrong size %d->%d", size, _obj->data_size);
        }
    }
}


void CCStruct_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    OPDumper_binary_setWithLength(dumper, _obj->data_bin, _obj->data_size);
}

SABool_t CCStruct_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    size_t len;
    if(OPLoader_binary_getLen(loader, token, &len))
    {
        struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
        if(len == _obj->data_size)
        {
            if(OPLoader_binary_get(loader, token, _obj->data_size, _obj->data_bin, &len))
            {
                return SABOOL_TRUE;
            }
        }
    }

    return SABOOL_FALSE;
}