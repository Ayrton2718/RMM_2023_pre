#include "cc_base_object.h"

#include "cc_dead_queue.h"

static void CCBaseObject_destructor_nolock(CC_obj obj);
static int CCBaseObject_referenceCount_nolock(CC_obj obj);
static CC_obj CCBaseObject_copy_nolock(CC_obj obj);
static SABool_t CCBaseObject_replace_nolock(CC_obj obj, CC_obj obj_from);
static size_t CCBaseObject_toHash_nolock(CC_obj obj);
static void CCBaseObject_debug_nolock(CC_obj obj, SABuffer_t* string);
static void CCBaseObject_dump_nolock(CC_obj obj,OPDumper_t dumper);
static SABool_t CCBaseObject_load_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token);
static SABool_t CCBaseObject_isLockerEnable_nolock(CC_obj obj);

void CCBaseObject_using_lock_callback(CC_obj obj);
void CCBaseObject_unusinge_lock_callback(CC_obj obj);
void CCBaseObject_using_unlock_callback(CC_obj obj);
void CCBaseObject_unusinge_unlock_callback(CC_obj obj);

CC_obj CCBaseObject_create(size_t size, CC_type_t type_id, CCBaseObject_destructor_calback_nolock_t destraructor, CCBaseObject_enableLocker_callback_t enableLocker, CCBaseObject_copy_callback_nolock_t copy, CCBaseObject_replace_callback_nolock_t replace, CCBaseObject_toHash_callback_nolock_t toHash, CCBaseObject_debug_callback_nolock_t debug, CCBaseObject_dump_callback_nolock_t dumper, CCBaseObject_load_callback_nolock_t loader)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)SAMEM_MALLOC(size);
    _obj->is_object_id = CC_OBJECT_ID;
    _obj->type_id = type_id;
    _obj->retain = 0;
    _obj->release = 0;

    _obj->locker = NULL;

    _obj->lock_callback = CCBaseObject_unusinge_lock_callback;
    _obj->unlock_callback = CCBaseObject_unusinge_unlock_callback;

    _obj->destructor_callback = destraructor;
    _obj->enableLocker_callback = enableLocker;
    _obj->copy_callback = copy;
    _obj->replace_callback = replace;
    _obj->toHash_callback = toHash;
    _obj->debug_callback = debug;
    _obj->dump_callback = dumper;
    _obj->load_callback = loader;

    return _obj;
}

SABool_t CCBaseObject_isTypeObject(CC_obj obj, CC_type_t id)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    if(CCBaseObject_isObject(obj))
    {
        if(_obj->type_id != id)
        {
            SALOG_ERROR("collection_class", "Type error. %s cannot use as %s!", CCType_toName(_obj->type_id), CCType_toName(id));
            return SABOOL_FALSE;
        }
    }else{
        return SABOOL_FALSE;
    }

    return SABOOL_TRUE;
}

SABool_t CCBaseObject_isObject(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    if(obj == NULL)
    {
        SALOG_ERROR_NOFMT("collection_class", "Type error. NULL pointer detected!");
        return SABOOL_FALSE;
    }

    if(_obj->is_object_id != CC_OBJECT_ID)
    {
        SALOG_ERROR_NOFMT("collection_class", "Type error. invited object!");
        return SABOOL_FALSE;
    }

    return SABOOL_TRUE;
}


CC_type_t CCBaseObject_getTypeId(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    CC_type_t type_id = CC_UNKNOWN_TYPE_ID;

    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        type_id = _obj->type_id;
        CCBaseObject_unlock(obj);
    }
    return type_id;
}


SABool_t CCBaseObject_checkObject(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    if(obj == NULL)
    {
        return SABOOL_FALSE;
    }

    if(_obj->is_object_id != CC_OBJECT_ID)
    {
        return SABOOL_FALSE;
    }

    return SABOOL_TRUE;
}


void CCBaseObject_retain_nolock(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    _obj->retain++;
}

void CCBaseObject_retain(CC_obj obj)
{
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        CCBaseObject_retain_nolock(obj);
        CCBaseObject_unlock(obj);
    }
}

void CCBaseObject_release(CC_obj obj)
{
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
        CCBaseObject_lock(obj);
        _obj->release++;
        
        if(CCBaseObject_referenceCount_nolock(obj) <= 0)
        {
            if(CCBaseObject_isLockerEnable_nolock(obj) == SABOOL_TRUE)
            {
                CCBaseObject_unlock(obj);
                CCDeadQueue_add(obj);
            }else{
                CCBaseObject_destructor_nolock(obj);
            }
        }else{
            CCBaseObject_unlock(obj);
        }
    }
}

int CCBaseObject_referenceCount(CC_obj obj)
{
    int count = 0;
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        count = CCBaseObject_referenceCount_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return count;
}

void CCBaseObject_lock(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    _obj->lock_callback(obj);
}

void CCBaseObject_unlock(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    _obj->unlock_callback(obj);
}

CC_obj CCBaseObject_copy(CC_obj obj)
{
    CC_obj copy_obj = NULL;
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        copy_obj = CCBaseObject_copy_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return copy_obj;
}

SABool_t CCBaseObject_replace(CC_obj obj, CC_obj obj_from)
{
    if(CCBaseObject_isObject(obj) && CCBaseObject_isObject(obj_from))
    {
        CCBaseObject_lock(obj);
        CCBaseObject_lock(obj_from);
        SABool_t result = CCBaseObject_replace_nolock(obj, obj_from);
        CCBaseObject_unlock(obj_from);
        CCBaseObject_unlock(obj);

        return result;
    }else{
        return SABOOL_FALSE;
    }
}

size_t CCBaseObject_toHash(CC_obj obj)
{
    size_t hash = 0;
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        hash = CCBaseObject_toHash_nolock(obj);
        CCBaseObject_unlock(obj);
    }
    return hash;
}

void CCBaseObject_debug(CC_obj obj, SABuffer_t* string)
{
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        CCBaseObject_debug_nolock(obj, string);
        CCBaseObject_unlock(obj);
    }
}

void CCBaseObject_dump(CC_obj obj,OPDumper_t dumper)
{
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        CCBaseObject_dump_nolock(obj, dumper);
        CCBaseObject_unlock(obj);
    }
}

SABool_t CCBaseObject_load(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    SABool_t is_success = SABOOL_FALSE;
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        is_success = CCBaseObject_load_nolock(obj, loader, token);
        CCBaseObject_unlock(obj);
    }
    return is_success;
}

void CCBaseObject_destructor(CC_obj obj)
{
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        CCBaseObject_destructor_nolock(obj);
    }
}

static void CCBaseObject_destructor_nolock(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;

    if(_obj->locker != NULL)
    {
        _obj->destructor_callback(obj);
        _obj->is_object_id = 0;

        SALOCKER_UNLOCK(_obj->locker);
        SALOCKER_DESTROY(_obj->locker);
        SAMEM_FREE(_obj->locker);

        SAMEM_FREE(obj);
    }else{
        _obj->destructor_callback(obj);
        _obj->is_object_id = 0;
        SAMEM_FREE(obj);
    }
}

static int CCBaseObject_referenceCount_nolock(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    return _obj->retain - _obj->release;
}

static CC_obj CCBaseObject_copy_nolock(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    CC_obj clone_obj = _obj->copy_callback(obj);
    return clone_obj;
}

SABool_t CCBaseObject_replace_nolock(CC_obj obj, CC_obj obj_from)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    CCBaseObject_t* _obj_from = (CCBaseObject_t*)obj_from;

    if(_obj->type_id != _obj_from->type_id)
    {
        return SABOOL_FALSE;
    }
    return _obj->replace_callback(_obj, _obj_from);
}

static size_t CCBaseObject_toHash_nolock(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    size_t hash = _obj->toHash_callback(obj);
    return hash;
}

static void CCBaseObject_debug_nolock(CC_obj obj, SABuffer_t* string)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    char info[64];
    char* is_enable_locker;
    if(_obj->locker == NULL)
    {
        is_enable_locker = (char*)"disable";
    }else{
        is_enable_locker = (char*)"enable";
    }
    sprintf(info, "{\"%s(%zu,%zu, %s)\":", CCType_toName(_obj->type_id), _obj->retain, _obj->release, is_enable_locker);
    SABuffer_add_stream(string, strlen(info), info);
    
    _obj->debug_callback(obj, string);
    SABuffer_add(string, "}");
}


static void CCBaseObject_dump_nolock(CC_obj obj,OPDumper_t dumper)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    _obj->dump_callback(obj, dumper);
}

static SABool_t CCBaseObject_load_nolock(CC_obj obj, OPLoader_t loader, OPType_token_t token)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    return _obj->load_callback(obj, loader, token);
}


SABool_t CCBaseObject_isLockerEnable(CC_obj obj)
{
    SABool_t is_enable = SABOOL_FALSE;
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);
        is_enable = CCBaseObject_isLockerEnable_nolock(obj);
        CCBaseObject_unlock(obj);
    }

    return is_enable;
}

SABool_t CCBaseObject_isLockerEnable_nolock(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    if(_obj->locker == NULL)
    {
        return SABOOL_FALSE;
    }else{
        return SABOOL_TRUE;
    }
}

void CCBaseObject_enableLocker(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    if(CCBaseObject_isObject(obj))
    {
        CCBaseObject_lock(obj);

        if(CCBaseObject_isLockerEnable_nolock(obj) == SABOOL_FALSE)
        {
            _obj->locker = (pthread_mutex_t*)SAMEM_MALLOC(sizeof(pthread_mutex_t));
            SALOCKER_INIT(_obj->locker, NULL);
            _obj->lock_callback = CCBaseObject_using_lock_callback;
            _obj->unlock_callback = CCBaseObject_using_unlock_callback;

            CCBaseObject_lock(obj);
        }

        _obj->enableLocker_callback(obj);
        CCBaseObject_unlock(obj);
    }
}

void CCBaseObject_using_lock_callback(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    SALOCKER_LOCK(_obj->locker);
}

void CCBaseObject_unusinge_lock_callback(CC_obj obj)
{
}

void CCBaseObject_using_unlock_callback(CC_obj obj)
{
    CCBaseObject_t* _obj = (CCBaseObject_t*)obj;
    SALOCKER_UNLOCK(_obj->locker);
}

void CCBaseObject_unusinge_unlock_callback(CC_obj obj)
{
}
