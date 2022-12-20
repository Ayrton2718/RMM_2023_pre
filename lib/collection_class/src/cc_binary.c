#include "cc_binary.h"

#include "cc_base_object.h"

struct CCBinary_t
{
    CCBaseObject_t object;

    SABuffer_t  binary;
    uint8_t* bin_buffer;
};


static size_t CCBinary_size_nolock(CC_obj obj);
static void CCBinary_add_nolock(CC_obj obj, const void* bin, size_t size);
static const uint8_t* CCBinary_getRaw_nolock(CC_obj obj);

void CCBinary_destructor_callback_nolock(CC_obj obj);
void CCBinary_enableLocker_callback_nolock(CC_obj obj);
CC_obj CCBinary_copy_callback_nolock(CC_obj obj);
SABool_t CCBinary_replace_callback_nolock(CC_obj obj, CC_obj obj_from);
size_t CCBinary_toHash_callbacdk_nolock(CC_obj obj);
void CCBinary_debug_callback_nolock(CC_obj obj, SABuffer_t* strings);
void CCBinary_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
SABool_t CCBinary_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


CC_obj CCBinary_createWithLength(const void* bin, size_t size)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)CCBaseObject_create(
        sizeof(struct CCBinary_t), 
        CC_BINARY_TYPE_ID, 
        CCBinary_destructor_callback_nolock, 
        CCBinary_enableLocker_callback_nolock,
        CCBinary_copy_callback_nolock, 
        CCBinary_replace_callback_nolock,
        CCBinary_toHash_callbacdk_nolock, 
        CCBinary_debug_callback_nolock, 
        CCBinary_dump_callback_nolock,
        CCBinary_load_callback_nolock);

    SABuffer_create(&_obj->binary, sizeof(char));
    if(bin != NULL)
    {
        SABuffer_add_stream(&_obj->binary, size, bin);
    }

    _obj->bin_buffer = NULL;
    return _obj;
}


size_t CCBinary_size(CC_obj obj)
{
    size_t result = 0;
    if(CCBaseObject_isTypeObject(obj, CC_BINARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        result = CCBinary_size_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return result;
}


void CCBinary_add(CC_obj obj, const void* bin, size_t size)
{
    if(CCBaseObject_isTypeObject(obj, CC_BINARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCBinary_add_nolock(obj, bin, size);
        CCBaseObject_unlock(obj);
    }
}


const uint8_t* CCBinary_getRaw(CC_obj obj)
{
    if(CCBaseObject_isTypeObject(obj, CC_BINARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        const uint8_t* result = CCBinary_getRaw_nolock(obj);
        CCBaseObject_unlock(obj);
        return result;
    }
    return NULL;
}


static size_t CCBinary_size_nolock(CC_obj obj)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;
    return SABuffer_count(&_obj->binary);
}

static void CCBinary_add_nolock(CC_obj obj, const void* bin, size_t size)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;
    SABuffer_add_stream(&_obj->binary, size, bin);
}

static const uint8_t* CCBinary_getRaw_nolock(CC_obj obj)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;

    size_t bin_size = SABuffer_count(&_obj->binary);

    if(_obj->bin_buffer == NULL)
    {
        _obj->bin_buffer = SAMEM_MALLOC(sizeof(uint8_t) * bin_size);
    }else{
        _obj->bin_buffer = SAMEM_REALLOC(_obj->bin_buffer, sizeof(uint8_t) * bin_size);
    }
    SABuffer_read_stream(&_obj->binary, 0, bin_size, _obj->bin_buffer);
    return _obj->bin_buffer;
}


void CCBinary_destructor_callback_nolock(CC_obj obj)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;

    SABuffer_destructor(&_obj->binary);

    if(_obj->bin_buffer != NULL)
    {
        SAMEM_FREE(_obj->bin_buffer);
    }
}

void CCBinary_enableLocker_callback_nolock(CC_obj obj)
{
}

CC_obj CCBinary_copy_callback_nolock(CC_obj obj)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;
    return CCBinary_createWithLength(SABuffer_getRaw(&_obj->binary), SABuffer_count(&_obj->binary));
}

SABool_t CCBinary_replace_callback_nolock(CC_obj obj, CC_obj obj_from)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;
    struct CCBinary_t* _obj_from = (struct CCBinary_t*)obj_from;

    SABuffer_removeAll(&_obj->binary);
    SABuffer_add_stream(&_obj->binary, SABuffer_count(&_obj->binary), SABuffer_getRaw(&_obj->binary));
    return SABOOL_TRUE;
}

size_t CCBinary_toHash_callbacdk_nolock(CC_obj obj)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;

    size_t sum = 0;

    const uint8_t* bin_raw = (const uint8_t*)SABuffer_getRaw(&_obj->binary);
    for(size_t i = 0; i < SABuffer_count(&_obj->binary); i++)
    {
        sum += bin_raw[i];
    }
    return sum;
}

void CCBinary_debug_callback_nolock(CC_obj obj, SABuffer_t* string)
{
    SABuffer_add(string, "{");
    SABuffer_add(string, "}");
}

void CCBinary_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;
    const uint8_t* bin = SABuffer_getRaw(&_obj->binary);
    size_t bin_size = SABuffer_count(&_obj->binary);
    OPDumper_binary_setWithLength(dumper, bin, bin_size);
}

SABool_t CCBinary_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    struct CCBinary_t* _obj = (struct CCBinary_t*)obj;
    SABool_t is_success = SABOOL_TRUE;
    uint8_t* raw;
    size_t len;

    is_success = SABOOL_AND(is_success, OPLoader_binary_getRaw(loader, token, &raw));
    is_success = SABOOL_AND(is_success, OPLoader_binary_getLen(loader, token, &len));

    if(is_success)
    {
        SABuffer_clear(&_obj->binary);
        SABuffer_add_stream(&_obj->binary, len, raw);
    }
    return is_success;
}
