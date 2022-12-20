#include "cc_json_serializer.h"

#include <object_parser/object_parser.h>

#include "cc_base_object.h"
#include "cc_string.h"

#include "cc_dictionary.h"
#include "cc_array.h"
#include "cc_bool.h"
#include "cc_double.h"
#include "cc_int_64.h"
#include "cc_binary.h"
#include "cc_null_object.h"


CC_obj CCJsonSerializer_dump(CC_obj obj, SABool_t visible)
{
    OPDumper_t dumper = OPJsonDumper_create(visible);
    CCBaseObject_dump(obj, dumper);
    CC_obj bin_obj = CCBinary_createWithLength((const uint8_t*)OPDumper_dumpPtr(dumper), OPDumper_dumpLength(dumper));
    CCBinary_add(bin_obj, "\0", sizeof("\0") - 1);
    OPDumper_destructor(dumper);
    return bin_obj;
}



SABool_t CCJsonSerializer_staticLoad(CC_obj obj, CC_obj bin_obj)
{
    OPLoader_t loader = OPJsonLoader_create();
    OPLoader_load(loader, (const uint8_t*)CCBinary_getRaw(bin_obj), CCBinary_size(bin_obj));
    OPLoader_destructor(loader);
    return CCBaseObject_load(obj, loader, OPLoader_getRoot(loader));
}



static CC_obj CCJsonSerializer_objectMaker(OPLoader_t loader, OPType_token_t token)
{
    CC_obj obj;

    switch(OPLoader_getType(loader, token))
    {
    case OPType_objectType_DICTIONARY:
        {
            obj = CCDictionary_create();
            size_t count;
            OPLoader_dictionary_getCount(loader, token, &count);
            for(size_t dic_i = 0; dic_i < count; dic_i++)
            {
                char key[128];
                OPType_token_t cur_token;
                OPLoader_dictionary_getAtIndex(loader, token, dic_i, key, &cur_token);
                CC_obj cur_obj = CCJsonSerializer_objectMaker(loader, cur_token);
                CCDictionary_setObject(obj, cur_obj, key);
            }
        }
        break;

    case OPType_objectType_ARRAY:
        {
            obj = CCArray_create();
            size_t count;
            OPLoader_array_getCount(loader, token, &count);
            for(size_t ar_i = 0; ar_i < count; ar_i++)
            {
                OPType_token_t cur_token;
                OPLoader_array_get(loader, token, ar_i, &cur_token);
                CC_obj cur_obj = CCJsonSerializer_objectMaker(loader, cur_token);
                CCArray_addObject(obj, cur_obj);
            }
        }
        break;
    
    case OPType_objectType_INT:
        {
            int64_t value;
            OPLoader_int_getSigned(loader, token, &value);
            obj = CCInt64_create(value);
        }
        break;

    case OPType_objectType_REAL:
        {
            double value;
            OPLoader_real_getDouble(loader, token, &value);
            obj = CCDouble_create(value);
        }
        break;
    
    case OPType_objectType_BOOL:
        {
            SABool_t value;
            OPLoader_bool_get(loader, token, &value);
            obj = CCBool_create(value);
        }
        break;

    case OPType_objectType_STR:
        {
            size_t len = 0;
            char* str;
            OPLoader_string_getLen(loader, token, &len);
            OPLoader_string_getRaw(loader, token, &str);
            obj = CCString_createWithLength(str, len);
        }
        break;

    case OPType_objectType_BIN:
        {
            size_t len = 0;
            uint8_t* bin;
            OPLoader_binary_getLen(loader, token, &len);
            OPLoader_binary_getRaw(loader, token, &bin);
            obj = CCBinary_createWithLength(bin, len);
        }
        break;
    
    case OPType_objectType_NULL:
        obj = CCNullObject_create();
        break;
    
    default:
        obj = CCNullObject_create();
        break;
    }

    return obj;
}


CC_obj CCJsonSerializer_dynamicLoad(CC_obj bin_obj)
{
    CC_obj obj = NULL;
    OPLoader_t loader = OPJsonLoader_create();
    if(OPLoader_load(loader, (const uint8_t*)CCBinary_getRaw(bin_obj), CCBinary_size(bin_obj)))
    {
        obj = CCJsonSerializer_objectMaker(loader, OPLoader_getRoot(loader));
    }

    OPLoader_destructor(loader);

    return obj;
}