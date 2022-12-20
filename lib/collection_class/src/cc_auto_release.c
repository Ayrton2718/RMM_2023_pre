#include "cc_auto_release.h"

#include "cc_base_object.h"
#include <pthread.h>

struct CCAutoRelease_scope_t
{
    SABuffer_t   objs;
};

struct CCAutoRelease_thread_t
{
    pthread_t               thread_id;
    SABuffer_t   scopes;
};

struct CCAutoRelease_owner_t
{
    pthread_mutex_t         locker;
    SABuffer_t   threads;
};

static struct CCAutoRelease_owner_t g_owner;

static struct CCAutoRelease_thread_t* CCAutoRelease_getCurrentThread_nolock(void); 

static void CCAutoRelease_startScope_nolock(void);
static void CCAutoRelease_doneScope_nolock(void);

static CC_obj CCAutoRelease_nolock(CC_obj obj);
static void __CCAutoRelease_addObjNolock_nolock(CC_obj obj);

void CCAutoRelease_ownerInit(void)
{
    SALOCKER_INIT(&g_owner.locker, NULL);

    SABuffer_create(&g_owner.threads, sizeof(struct CCAutoRelease_thread_t));
}


static struct CCAutoRelease_thread_t* CCAutoRelease_getCurrentThread_nolock(void)
{
    struct CCAutoRelease_thread_t* result = NULL;
    for(size_t i = 0; i < SABuffer_count(&g_owner.threads); i++)
    {
        struct CCAutoRelease_thread_t* thread_info = (struct CCAutoRelease_thread_t*)SABuffer_readAtIndex_pointer(&g_owner.threads, i); 
        if(pthread_equal(pthread_self(), thread_info->thread_id) != 0)
        {
            result = thread_info;
            break;
        }
    }
    return result;
}


void CCAutoRelease_startScope(void)
{
    SALOCKER_LOCK(&g_owner.locker);
    CCAutoRelease_startScope_nolock();
    SALOCKER_UNLOCK(&g_owner.locker);
}


void CCAutoRelease_doneScope(void)
{
    SALOCKER_LOCK(&g_owner.locker);
    CCAutoRelease_doneScope_nolock();
    SALOCKER_UNLOCK(&g_owner.locker);
}


CC_obj CCAutoRelease_add(CC_obj obj)
{
    if(CCBaseObject_isObject(obj))
    {
        SALOCKER_LOCK(&g_owner.locker);
        obj = CCAutoRelease_nolock(obj);
        SALOCKER_UNLOCK(&g_owner.locker);
    }

    return obj;
}

void __CCAutoRelease_addObjNolock(CC_obj obj)
{
    SALOCKER_LOCK(&g_owner.locker);
    __CCAutoRelease_addObjNolock_nolock(obj);
    SALOCKER_UNLOCK(&g_owner.locker);
}


static void CCAutoRelease_startScope_nolock(void)
{
    struct CCAutoRelease_thread_t* current = CCAutoRelease_getCurrentThread_nolock();
    if(current == NULL)
    {
        struct CCAutoRelease_thread_t thread_info;
        thread_info.thread_id = pthread_self();
        SABuffer_create(&thread_info.scopes, sizeof(struct CCAutoRelease_scope_t));

        SABuffer_add(&g_owner.threads, &thread_info);

        current = (struct CCAutoRelease_thread_t*)SABuffer_readLast_pointer(&g_owner.threads);
    }

    struct CCAutoRelease_scope_t scope;
    SABuffer_create(&scope.objs, sizeof(CC_obj));
    SABuffer_add(&current->scopes, &scope);
}


static void CCAutoRelease_doneScope_nolock(void)
{
    struct CCAutoRelease_thread_t* current = CCAutoRelease_getCurrentThread_nolock();
    if(current == NULL)
    {
        return;
    }

    struct CCAutoRelease_scope_t* scope = (struct CCAutoRelease_scope_t*)SABuffer_readLast_pointer(&current->scopes);
    if(scope != NULL)
    {
        for(size_t i = 0; i < SABuffer_count(&scope->objs); i++)
        {
            CC_obj* release_obj = (CC_obj*)SABuffer_readAtIndex_pointer(&scope->objs, i);
            CCBaseObject_release(*release_obj);
        }
        SABuffer_destructor(&scope->objs);
        SABuffer_removeLast(&current->scopes);
    }else{
        SALOG_ERROR_NOFMT("collection_class", "None scope! (when autorelease scope done.)");
    }
}


static CC_obj CCAutoRelease_nolock(CC_obj obj)
{
    struct CCAutoRelease_thread_t* current = CCAutoRelease_getCurrentThread_nolock();
    if(current == NULL)
    {
        SALOG_ERROR_NOFMT("collection_class", "None scope! (when object adding to autorelease.)");
        return obj;
    }

    struct CCAutoRelease_scope_t* scope = (struct CCAutoRelease_scope_t*)SABuffer_readLast_pointer(&current->scopes);

    if(scope != NULL)
    {
        CCBaseObject_retain(obj);
        SABuffer_add(&scope->objs, &obj);
    }else{
        SALOG_ERROR_NOFMT("collection_class", "None scope! (when object adding to autorelease.)");
    }
    return obj;
}


static void __CCAutoRelease_addObjNolock_nolock(CC_obj obj)
{
    struct CCAutoRelease_thread_t* current = CCAutoRelease_getCurrentThread_nolock();
    if(current == NULL)
    {
        SALOG_ERROR_NOFMT("collection_class", "None scope! (when object adding to autorelease.)");
        return;
    }

    struct CCAutoRelease_scope_t* scope = (struct CCAutoRelease_scope_t*)SABuffer_readLast_pointer(&current->scopes);

    if(scope != NULL)
    {
        CCBaseObject_retain_nolock(obj);
        SABuffer_add(&scope->objs, &obj);
    }else{
        SALOG_ERROR_NOFMT("collection_class", "None scope! (when object adding to autorelease.)");
    }
}
