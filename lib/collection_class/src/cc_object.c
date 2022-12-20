#include "cc_object.h"

#include "cc_base_object.h"
#include "cc_string.h"

const char* CCObject_getType(CC_obj obj)
{
    CC_type_t type_id = CCBaseObject_getTypeId(obj);
    return CCType_toName(type_id);
}

void CCObject_retain(CC_obj obj)
{
    CCBaseObject_retain(obj);
}

void CCObject_release(CC_obj obj)
{
    CCBaseObject_release(obj);
}

void CCObject_enableThreadSafe(CC_obj obj)
{
    CCBaseObject_enableLocker(obj);
}

CC_obj CCObject_copy(CC_obj obj)
{
    return CCBaseObject_copy(obj);
}

SABool_t CCObject_replace(CC_obj obj, CC_obj obj_from)
{
    return CCBaseObject_replace(obj, obj_from);
}

SABool_t CCObject_isObject(CC_obj obj)
{
    return CCBaseObject_checkObject(obj);
}

CC_obj CCObject_info(CC_obj obj)
{
    SABuffer_t string;
    SABuffer_create(&string, sizeof(char));
    CCBaseObject_debug(obj, &string);
    SABuffer_add(&string, "");
    CC_obj str_obj = CCString_create((const char*)SABuffer_getRaw(&string));
    SABuffer_destructor(&string);
    return str_obj;
}
