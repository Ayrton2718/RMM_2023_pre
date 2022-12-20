#include "cc_bool.h"

#include "cc_raw_binary.h"

static void CCBool_set_nolock(CC_obj obj, SABool_t set_value);
static SABool_t CCBool_get_nolock(CC_obj obj);

void CCBool_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
SABool_t CCBool_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


CC_obj CCBool_create(SABool_t init_value)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)CCRawBinary_create(sizeof(SABool_t), CC_BOOLEAN_TYPE_ID, CCBool_dump_callback_nolock, CCBool_load_callback_nolock);
    *(SABool_t*)_obj->data_bin = init_value;
    return _obj;
}

void CCBool_set(CC_obj obj, SABool_t set_value)
{
    if(CCBaseObject_isTypeObject(obj, CC_BOOLEAN_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCBool_set_nolock(obj, set_value);
        CCBaseObject_unlock(obj);
    }
}

SABool_t CCBool_get(CC_obj obj)
{
    SABool_t value = 0;
    if(CCBaseObject_isTypeObject(obj, CC_BOOLEAN_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        value = CCBool_get_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return value;
}


static void CCBool_set_nolock(CC_obj obj, SABool_t set_value)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    *(SABool_t*)_obj->data_bin = set_value;
}

static SABool_t CCBool_get_nolock(CC_obj obj)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    return *(SABool_t*)_obj->data_bin;
}


void CCBool_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
{
    OPDumper_bool_set(dumper, CCBool_get_nolock(obj));
}

SABool_t CCBool_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    SABool_t value;
    if(OPLoader_bool_get(loader, token, &value))
    {
        CCBool_set_nolock(obj, value);
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}
