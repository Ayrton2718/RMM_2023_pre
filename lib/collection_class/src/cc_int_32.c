#include "cc_int_32.h"

#include "cc_raw_binary.h"

static void CCInt32_set_nolock(CC_obj obj, int32_t set_value);
static int32_t CCInt32_get_nolock(CC_obj obj);

void CCInt32_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
SABool_t CCInt32_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


CC_obj CCInt32_create(int32_t init_value)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)CCRawBinary_create(sizeof(int32_t), CC_INT32_TYPE_ID, CCInt32_dump_callback_nolock, CCInt32_load_callback_nolock);
    *(int32_t*)_obj->data_bin = init_value;
    return _obj;
}

void CCInt32_set(CC_obj obj, int32_t set_value)
{
    if(CCBaseObject_isTypeObject(obj, CC_INT32_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCInt32_set_nolock(obj, set_value);
        CCBaseObject_unlock(obj);
    }
}

int32_t CCInt32_get(CC_obj obj)
{
    int32_t value = 0;
    if(CCBaseObject_isTypeObject(obj, CC_INT32_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        value = CCInt32_get_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return value;
}


static void CCInt32_set_nolock(CC_obj obj, int32_t set_value)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    *(int32_t*)_obj->data_bin = set_value;
}

static int32_t CCInt32_get_nolock(CC_obj obj)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    return *(int32_t*)_obj->data_bin;
}


void CCInt32_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
{
    OPDumper_int_setSigned(dumper, CCInt32_get_nolock(obj));
}

SABool_t CCInt32_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    int64_t value = 0;
    if(OPLoader_int_getSigned(loader, token, &value))
    {
        CCInt32_set_nolock(obj, (int32_t)value);
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}
