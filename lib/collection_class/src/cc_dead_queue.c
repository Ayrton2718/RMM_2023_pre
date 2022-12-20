#include "cc_dead_queue.h"

#include "cc_base_object.h"
#include <pthread.h>
#include <unistd.h>

#include <std_api/std_api.h>

#define CC_DEAD_QUEUE_QUEUE_COUNT   (256)
#define CC_DEAD_QUEUE_DEADLINE      (500) //ms


struct CCDeadQueue_objTimeInfo_t
{
    clock_t added_time;
    CC_obj obj;
};

struct CCDeadQueue_t
{
    pthread_mutex_t     locker;
    pthread_cond_t      set_cond;

    pthread_t           thread;

    SAQueue_t           queue;
    struct CCDeadQueue_objTimeInfo_t queue_buffer[CC_DEAD_QUEUE_QUEUE_COUNT];
};

static struct CCDeadQueue_t g_root;


void* CCDeadQueue_thread(void* data);


void CCDeadQueue_initializer(void)
{
    SAQueue_init(&g_root.queue, sizeof(struct CCDeadQueue_objTimeInfo_t), CC_DEAD_QUEUE_QUEUE_COUNT, g_root.queue_buffer);

    SALOCKER_INIT(&g_root.locker, NULL);
    SACOND_INIT(&g_root.set_cond, NULL);

    SATHREAD_CREATE("dead_queue", &g_root.thread, NULL, CCDeadQueue_thread, NULL);
}

static inline clock_t CCDeadQueue_getMillis(void)
{
    return clock() / (CLOCKS_PER_SEC / 1000);
}


void CCDeadQueue_add(CC_obj obj)
{
    struct CCDeadQueue_objTimeInfo_t time_info;
    time_info.added_time = CCDeadQueue_getMillis();
    time_info.obj = obj;

    SABool_t is_filled = SABOOL_FALSE;
    struct CCDeadQueue_objTimeInfo_t fillout;

    SALOCKER_LOCK(&g_root.locker);
    if(SAQueue_isFill(&g_root.queue))
    {
        is_filled = SABOOL_TRUE;
        SAQueue_get(&g_root.queue, &fillout);
    }    
    SAQueue_add(&g_root.queue, &time_info);
    SALOCKER_UNLOCK(&g_root.locker);
    
    SACOND_SIGNAL(&g_root.set_cond);

    if(is_filled == SABOOL_TRUE)
    {
        // CCLOG_INFO("fill kill. %p", fillout.obj);
        CCBaseObject_destructor(fillout.obj);
    }
}

void* CCDeadQueue_thread(void* data)
{
    while(1)
    {
        struct CCDeadQueue_objTimeInfo_t time_info;
        int is_available = 1;
        SALOCKER_LOCK(&g_root.locker);
        if(SAQueue_get(&g_root.queue, &time_info) != 1)
        {
            SACOND_WAIT(&g_root.set_cond, &g_root.locker);
            is_available = SAQueue_get(&g_root.queue, &time_info);
        }
        SALOCKER_UNLOCK(&g_root.locker);

        if(is_available == 1 && CCBaseObject_isObject(time_info.obj) == SABOOL_TRUE)
        {
            // CCLOG_INFO("ok");
            if(CCDeadQueue_getMillis() < time_info.added_time + CC_DEAD_QUEUE_DEADLINE)
            {
                usleep(time_info.added_time + CC_DEAD_QUEUE_DEADLINE - CCDeadQueue_getMillis());
            }

            if(CCBaseObject_referenceCount(time_info.obj) <= 0)
            {
                // CCLOG_INFO("kill %p", time_info.obj);
                CCBaseObject_destructor(time_info.obj);
            }else{
                // CCLOG_INFO("no kill %p", time_info.obj);
            }
        }else{
            SATHREAD_YIELD();
        }
    }

    return NULL;
}
