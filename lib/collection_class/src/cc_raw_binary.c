#include "cc_raw_binary.h"


void CCRawBinary_destructor_callback_nolock(CC_obj obj);
void CCRawBinary_enableLocker_callback_nolock(CC_obj obj);
CC_obj CCRawBinary_copy_callback_nolock(CC_obj obj);
SABool_t CCRawBinary_replace_callback_nolock(CC_obj obj, CC_obj obj_from);
size_t CCRawBinary_toHash_callback_nolock(CC_obj obj);
void CCRawBinary_debug_callback_nolock(CC_obj obj, SABuffer_t* strings);


CC_obj CCRawBinary_create(size_t data_size, CC_type_t type_id, CCBaseObject_dump_callback_nolock_t dumper, CCBaseObject_load_callback_nolock_t loader)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)CCBaseObject_create(
        sizeof(struct CCRawBinary_t), 
        type_id, 
        CCRawBinary_destructor_callback_nolock, 
        CCRawBinary_enableLocker_callback_nolock,
        CCRawBinary_copy_callback_nolock, 
        CCRawBinary_replace_callback_nolock,
        CCRawBinary_toHash_callback_nolock, 
        CCRawBinary_debug_callback_nolock, 
        dumper,
        loader);

    _obj->data_size = data_size;
    _obj->data_bin = SAMEM_MALLOC(_obj->data_size);

    return _obj;
}


void CCRawBinary_destructor_callback_nolock(CC_obj obj)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    SAMEM_FREE(_obj->data_bin);
}


void CCRawBinary_enableLocker_callback_nolock(CC_obj obj)
{
}


CC_obj CCRawBinary_copy_callback_nolock(CC_obj obj)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    struct CCRawBinary_t* _copy = (struct CCRawBinary_t*)CCRawBinary_create(_obj->data_size, _obj->object.type_id, _obj->object.dump_callback, _obj->object.load_callback);
    memcpy(_copy->data_bin, _obj->data_bin, _obj->data_size);
    return _copy;
}


SABool_t CCRawBinary_replace_callback_nolock(CC_obj obj, CC_obj obj_from)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    struct CCRawBinary_t* _replace = (struct CCRawBinary_t*)obj_from;

    if(_obj->data_size == _replace->data_size)
    {
        memcpy(_obj->data_bin, _replace->data_bin, _obj->data_size);
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}


size_t CCRawBinary_toHash_callback_nolock(CC_obj obj)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;

    size_t sum = 0;
    const uint8_t* hash_buff = (const uint8_t*)&_obj->data_bin;

    for(size_t i = 0; i < _obj->data_size; i++)
    {
        sum += hash_buff[i];
    }

    return sum;
}


void CCRawBinary_debug_callback_nolock(CC_obj obj, SABuffer_t* strings)
{
    SABuffer_add(strings, "{");
    SABuffer_add(strings, "}");
}
