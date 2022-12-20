#include "cc_null_object.h"

#include "cc_raw_binary.h"

// void CCNullObject_toJson_callback_nolock(CC_obj obj, SABuffer_t* string, SABool_t visible, size_t indent);
// SABool_t CCNullObject_loadJson_callback_nolock(CC_obj obj, struct CCJsonParser_t* parser, struct CCJsonParser_jsonToken_t* json_token);

void CCNullObject_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
SABool_t CCNullObject_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


CC_obj CCNullObject_create(void)
{
    struct CCRawBinary_t* _obj = (struct CCRawBinary_t*)CCRawBinary_create(sizeof(uint8_t), CC_NULL_TYPE_ID, CCNullObject_dump_callback_nolock, CCNullObject_load_callback_nolock);
    return _obj;
}


void CCNullObject_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
{
    OPDumper_null(dumper);
}


SABool_t CCNullObject_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    if(OPLoader_getType(loader, token) == OPType_objectType_NULL)
    {
        return SABOOL_TRUE;
    }else{
        return SABOOL_FALSE;
    }
}

