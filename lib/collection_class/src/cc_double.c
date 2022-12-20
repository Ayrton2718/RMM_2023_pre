#include "cc_double.h"

#include "cc_raw_binary.h"

static void CCDouble_set_nolock(CC_obj obj, double set_value);
static double CCDouble_get_nolock(CC_obj obj);

void CCDouble_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
SABool_t CCDouble_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


CC_obj CCDouble_create(double init_value)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)CCRawBinary_create(sizeof(double), CC_DOUBLE_TYPE_ID, CCDouble_dump_callback_nolock, CCDouble_load_callback_nolock);
    *(double*)_obj->data_bin = init_value;
    return _obj;
}

void CCDouble_set(CC_obj obj, double set_value)
{
    if(CCBaseObject_isTypeObject(obj, CC_DOUBLE_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCDouble_set_nolock(obj, set_value);
        CCBaseObject_unlock(obj);
    }
}

double CCDouble_get(CC_obj obj)
{
    double value = 0;
    if(CCBaseObject_isTypeObject(obj, CC_DOUBLE_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        value = CCDouble_get_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return value;
}


static void CCDouble_set_nolock(CC_obj obj, double set_value)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    *(double*)_obj->data_bin = set_value;
}

static double CCDouble_get_nolock(CC_obj obj)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)obj;
    return *(double*)_obj->data_bin;
}


void CCDouble_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
{
    OPDumper_real_setDouble(dumper, CCDouble_get_nolock(obj));
}

SABool_t CCDouble_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    double value = 0;
    if(OPLoader_real_getDouble(loader, token, &value))
    {
        CCDouble_set_nolock(obj, value);
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}
