#include "cc_dictionary.h"

#include "cc_base_object.h"

#include "cc_array.h"
#include "cc_string.h"


struct CCDictionary_pair_t
{
    char        key[64];
    CC_obj      obj;
};

struct CCDictionary_t
{
    CCBaseObject_t object;

    SABuffer_t pair_array;
};


static size_t CCDictionary_count_nolock(CC_obj obj);
static CC_obj CCDictionary_allKeys_nolock(CC_obj obj);
static SABool_t CCDictionary_containsKey_nolock(CC_obj obj, const char* key);
static CC_obj CCDictionary_allKeysForObject_nolock(CC_obj obj, CC_obj target_obj);
static CC_obj CCDictionary_objectForKey_nolock(CC_obj obj, const char* key);
static CC_obj CCDictionary_objectAtIndex_nolock(CC_obj obj, size_t index);
static void CCDictionary_setObject_nolock(CC_obj obj, CC_obj set_obj, const char* key);
static void CCDictionary_removeObjectForKey_nolock(CC_obj obj, const char* key);
static void CCDictionary_removeObjectForObj_nolock(CC_obj obj, CC_obj target_obj);
static void CCDictionary_removeAllObjects_nolock(CC_obj obj);
static CC_obj CCDictionary_objectForKey_withIndex_nolock(CC_obj obj, const char* key, size_t* index_out);

void CCDictionary_destructor_callback_nolock(CC_obj obj);
void CCDictionary_enableLocker_callback_nolock(CC_obj obj);
CC_obj CCDictionary_copy_callback_nolock(CC_obj obj);
SABool_t CCDictionary_replace_callback_nolock(CC_obj obj, CC_obj obj_from);
size_t CCDictionary_toHash_callback_nolock(CC_obj obj);
void CCDictionary_debug_callback_nolock(CC_obj obj, SABuffer_t* strings);
// void CCDictionary_toJson_callback_nolock(CC_obj obj, SABuffer_t* string, SABool_t visible, size_t indent);
// SABool_t CCDictionary_loadJson_callback_nolock(CC_obj obj, struct CCJsonParser_t* parser, struct CCJsonParser_jsonToken_t* json_token);
void CCDictionary_dump_callback_nolock(CC_obj obj, OPDumper_t dumper);
SABool_t CCDictionary_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);


CC_obj CCDictionary_create(void)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)CCBaseObject_create(
        sizeof(struct CCDictionary_t),
        CC_DICTIONARY_TYPE_ID,
        CCDictionary_destructor_callback_nolock,
        CCDictionary_enableLocker_callback_nolock,
        CCDictionary_copy_callback_nolock,
        CCDictionary_replace_callback_nolock,
        CCDictionary_toHash_callback_nolock,
        CCDictionary_debug_callback_nolock,
        CCDictionary_dump_callback_nolock,
        CCDictionary_load_callback_nolock);

    SABuffer_create(&_obj->pair_array, sizeof(struct CCDictionary_pair_t));

    return _obj;
}

size_t CCDictionary_count(CC_obj obj)
{
    size_t count = 0;
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        count = CCDictionary_count_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return count;
}

CC_obj CCDictionary_allKeys(CC_obj obj)
{
    CC_obj key_array = NULL;
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        key_array = CCDictionary_allKeys_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return key_array;
}

SABool_t CCDictionary_containsKey(CC_obj obj, const char* key)
{
    SABool_t is_contain = SABOOL_FALSE;
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        is_contain = CCDictionary_containsKey_nolock(obj, key);
        CCBaseObject_unlock(obj);
    }
    return is_contain;
}

CC_obj CCDictionary_allKeysForObject(CC_obj obj, CC_obj target_obj)
{
    CC_obj key_array = NULL;
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCDictionary_allKeysForObject_nolock(obj, target_obj);
        CCBaseObject_unlock(obj);
    }
    return key_array;
}

CC_obj CCDictionary_objectForKey(CC_obj obj, const char* key)
{
    CC_obj same_key_obj = NULL;
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        same_key_obj = CCDictionary_objectForKey_nolock(obj, key);
        CCBaseObject_unlock(obj);
    }
    return same_key_obj;
}

CC_obj CCDictionary_objectAtIndex(CC_obj obj, size_t index)
{
    CC_obj same_key_obj = NULL;
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        same_key_obj = CCDictionary_objectAtIndex_nolock(obj, index);
        CCBaseObject_unlock(obj);
    }
    return same_key_obj;
}

void CCDictionary_setObject(CC_obj obj, CC_obj set_obj, const char* key)
{
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCDictionary_setObject_nolock(obj, set_obj, key);
        CCBaseObject_unlock(obj);
    }
}

void CCDictionary_removeObjectForKey(CC_obj obj, const char* key)
{
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCDictionary_removeObjectForKey_nolock(obj, key);
        CCBaseObject_unlock(obj);
    }
}

void CCDictionary_removeObjectForObj(CC_obj obj, CC_obj target_obj)
{
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCDictionary_removeObjectForObj_nolock(obj, target_obj);
        CCBaseObject_unlock(obj);
    }
}

void CCDictionary_removeAllObjects(CC_obj obj)
{
    if(CCBaseObject_isTypeObject(obj, CC_DICTIONARY_TYPE_ID))
    {
        CCBaseObject_lock(obj);
        CCDictionary_removeAllObjects_nolock(obj);
        CCBaseObject_unlock(obj);
    }
}


static size_t CCDictionary_count_nolock(CC_obj obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;
    return SABuffer_count(&_obj->pair_array);
}

static SABool_t CCDictionary_containsKey_nolock(CC_obj obj, const char* key)
{
    size_t index_tmp;
    CC_obj same_key_obj = CCDictionary_objectForKey_withIndex_nolock(obj, key, &index_tmp);
    SABool_t result;
    if(same_key_obj == NULL)
    {
        result = SABOOL_FALSE;
    }else{
        result = SABOOL_TRUE;
    }
    return result;
}

static CC_obj CCDictionary_allKeys_nolock(CC_obj obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    CC_obj array_obj = CCArray_create();
    for(size_t i = 0; i < SABuffer_count(&_obj->pair_array); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        CCArray_addObject(array_obj, CCString_create(pair->key));
    }

    return array_obj;
}

static CC_obj CCDictionary_allKeysForObject_nolock(CC_obj obj, CC_obj target_obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    size_t target_hash = CCBaseObject_toHash(target_obj);

    CC_obj array_obj = CCArray_create();
    for(size_t i = 0; i < SABuffer_count(&_obj->pair_array); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        if(CCBaseObject_toHash(pair->obj) == target_hash)
        {
            CCArray_addObject(array_obj, CCString_create(pair->key));
        }
    }

    return array_obj;
}

static CC_obj CCDictionary_objectForKey_nolock(CC_obj obj, const char* key)
{
    size_t index_tmp;
    return CCDictionary_objectForKey_withIndex_nolock(obj, key, &index_tmp);
}

static CC_obj CCDictionary_objectAtIndex_nolock(CC_obj obj, size_t index)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;
    
    struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, index);
    return pair->obj;
}

static void CCDictionary_setObject_nolock(CC_obj obj, CC_obj set_obj, const char* key)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    size_t index;
    CC_obj same_key_obj = CCDictionary_objectForKey_withIndex_nolock(obj, key, &index);

    if(same_key_obj == NULL)
    {
        size_t key_len = strlen(key);
        if(key_len == 0)
        {
            SALOG_ERROR("collection_class", "Key length is zero!");
            return;
        }
        struct CCDictionary_pair_t pair;
        memcpy(pair.key, key, sizeof(char) * key_len);
        pair.key[key_len] = 0x00;
        pair.obj = set_obj;

        CCBaseObject_retain(set_obj);
        SABuffer_add(&_obj->pair_array, &pair);
    }else{
        CCBaseObject_release(same_key_obj);

        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, index);
        CCBaseObject_retain(set_obj);
        pair->obj = set_obj;
    }
}

static void CCDictionary_removeObjectForKey_nolock(CC_obj obj, const char* key)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    size_t index;
    CC_obj same_key_obj = CCDictionary_objectForKey_withIndex_nolock(obj, key, &index);

    if(same_key_obj != NULL)
    {
        CCBaseObject_release(same_key_obj);

        SABuffer_removeAtIndex(&_obj->pair_array, index);
    }
}

static void CCDictionary_removeObjectForObj_nolock(CC_obj obj, CC_obj target_obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    for(size_t i = 0; i < CCDictionary_count_nolock(obj); i++)
    {
        if(CCDictionary_objectAtIndex_nolock(obj, i) == target_obj)
        {
            CCBaseObject_release(target_obj);
            SABuffer_removeAtIndex(&_obj->pair_array, i);
        }
    }
}

static void CCDictionary_removeAllObjects_nolock(CC_obj obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;
    
    for(size_t i = 0; i < SABuffer_count(&_obj->pair_array); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        CCBaseObject_release(pair->obj);
    }

    SABuffer_removeAll(&_obj->pair_array);
}

static CC_obj CCDictionary_objectForKey_withIndex_nolock(CC_obj obj, const char* key, size_t* index_out)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    CC_obj same_key_obj = NULL;
    for(size_t i = 0; i < SABuffer_count(&_obj->pair_array); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        if(pair->key[0] == key[0])
        {
            size_t pair_key_len = strlen(pair->key);
            if(pair_key_len == strlen(key))
            {
                if(memcmp(pair->key, key, pair_key_len) == 0)
                {
                    *index_out = i;
                    same_key_obj = pair->obj;
                    break;
                }
            }
        }
    }
    return same_key_obj;
}


void CCDictionary_destructor_callback_nolock(CC_obj obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;
    CCDictionary_removeAllObjects_nolock(obj);

    SABuffer_destructor(&_obj->pair_array);
}

void CCDictionary_enableLocker_callback_nolock(CC_obj obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    for(size_t i = 0; i < CCDictionary_count_nolock(obj); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        CCBaseObject_enableLocker(pair->obj);
    }
}

CC_obj CCDictionary_copy_callback_nolock(CC_obj obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    struct CCDictionary_t*  copy_obj = (struct CCDictionary_t*)CCDictionary_create();

    for(size_t i = 0; i < CCDictionary_count_nolock(obj); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        CC_obj current_copy_obj = CCBaseObject_copy(pair->obj);
        CCDictionary_setObject_nolock(copy_obj, current_copy_obj, pair->key);
    }

    return copy_obj;
}


SABool_t CCDictionary_replace_callback_nolock(CC_obj obj, CC_obj obj_from)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;
    struct CCDictionary_t* _obj_from = (struct CCDictionary_t*)obj_from;

    for(size_t i = 0; i < CCDictionary_count(_obj_from); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj_from->pair_array, i);

        CC_obj replace_obj = CCDictionary_objectForKey_nolock(_obj, pair->key);
        if(replace_obj == NULL)
        {
            return SABOOL_FALSE;
        }

        if(CCBaseObject_replace(replace_obj, pair->obj) == SABOOL_FALSE)
        {
            return SABOOL_FALSE;
        }

    }

    return SABOOL_TRUE;
}


size_t CCDictionary_toHash_callback_nolock(CC_obj obj)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    size_t hash_sum = 0;

    for(size_t i = 0; i < SABuffer_count(&_obj->pair_array); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        size_t key_hash = 0;
        for(size_t key_index = 0; pair->key[key_index] != 0; i++)
        {
            key_hash += pair->key[key_index];
        }
        
        hash_sum += CCBaseObject_toHash(pair->obj) ^ key_hash;
    }
    return hash_sum;
}

void CCDictionary_debug_callback_nolock(CC_obj obj, SABuffer_t* string)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    SABuffer_add(string, "{");
    for(size_t i = 0; i < CCDictionary_count_nolock(obj); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        char info[68];
        sprintf(info, "\"%s\":", pair->key);
        SABuffer_add_stream(string, strlen(info), info);
        
        CCBaseObject_debug(pair->obj, string);
        if(i < CCDictionary_count_nolock(obj) - 1)
        {
            SABuffer_add(string, ",");
        }
    }
    SABuffer_add(string, "}");
}


void CCDictionary_dump_callback_nolock(CC_obj obj, OPDumper_t dumper)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;
    OPDumper_dictionary_start(dumper, CCDictionary_count_nolock(obj));
    for(size_t i = 0; i < CCDictionary_count_nolock(obj); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        OPDumper_dictionary_setKey(dumper, pair->key);
        CCBaseObject_dump(pair->obj, dumper);
    }
    OPDumper_dictionary_end(dumper);
}

SABool_t CCDictionary_load_callback_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    struct CCDictionary_t* _obj = (struct CCDictionary_t*)obj;

    SABool_t is_success = SABOOL_TRUE;

    for(size_t i = 0; i < CCDictionary_count_nolock(obj); i++)
    {
        struct CCDictionary_pair_t* pair = (struct CCDictionary_pair_t*)SABuffer_readAtIndex_pointer(&_obj->pair_array, i);
        OPType_token_t cur_token;
        if(OPLoader_dictionary_get(loader, token, pair->key, &cur_token))
        {
            is_success = SABOOL_AND(is_success, CCBaseObject_load(pair->obj, loader, cur_token));
        }else{
            is_success = SABOOL_FALSE;
        }
    }

    return is_success;
}
