#include "cc_int_64.h"

#include "cc_raw_binary.h"

static void CCInt64_set_nolock(CC_obj obj, int64_t set_value);
static int64_t CCInt64_get_nolock(CC_obj obj);

void CCInt64_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
SABool_t CCInt64_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


CC_obj CCInt64_create(int64_t init_value)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)CCRawBinary_create(sizeof(int64_t), CC_INT64_TYPE_ID, CCInt64_dump_callback_nolock, CCInt64_load_callback_nolock);
    *(int64_t*)_obj->data_bin = init_value;
    return _obj;
}

void CCInt64_set(CC_obj obj, int64_t set_value)
{
    if(CCBaseObject_isTypeObject(obj, CC_INT64_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCInt64_set_nolock(obj, set_value);
        CCBaseObject_unlock(obj);
    }
}

int64_t CCInt64_get(CC_obj obj)
{
    int64_t value = 0;
    if(CCBaseObject_isTypeObject(obj, CC_INT64_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        value = CCInt64_get_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return value;
}


static void CCInt64_set_nolock(CC_obj obj, int64_t set_value)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    *(int64_t*)_obj->data_bin = set_value;

}

static int64_t CCInt64_get_nolock(CC_obj obj)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    return *(int64_t*)_obj->data_bin;
}


void CCInt64_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
{
    OPDumper_int_setSigned(dumper, CCInt64_get_nolock(obj));
}

SABool_t CCInt64_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    int64_t value = 0;
    if(OPLoader_int_getSigned(loader, token, &value))
    {
        CCInt64_set_nolock(obj, value);
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}
