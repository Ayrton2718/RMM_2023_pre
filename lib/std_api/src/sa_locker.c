#include "sa_locker.h"

#include <errno.h>
#include "system/sa_sys_log.h"


void SALocker_init(const char* file, size_t line, pthread_mutex_t* mutex, pthread_mutexattr_t* attr)
{
    int result = pthread_mutex_init(mutex, attr);
    if(result != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed mutex_init!", (int)result);
    }
}

void SALocker_destroy(const char* file, size_t line, pthread_mutex_t* mutex)
{
    int result = pthread_mutex_destroy(mutex);
    if(result != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed mutex_destroy!", (int)result);
    }
}

void SALocker_lock(const char* file, size_t line, pthread_mutex_t* mutex)
{
    int result = pthread_mutex_lock(mutex);
    if(result != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed mutex_lock!", (int)result);
    }
}

void SALocker_unlock(const char* file, size_t line, pthread_mutex_t* mutex)
{
    int result = pthread_mutex_unlock(mutex);
    if(result != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed mutex_unlock!", (int)result);
    }
}


void SACond_init(const char* file, size_t line, pthread_cond_t* cond, pthread_condattr_t* attr)
{
    int result = pthread_cond_init(cond, attr);
    if(result != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed cond_init!", (int)result);
    }
}

void SACond_destroy(const char* file, size_t line, pthread_cond_t* cond)
{
    int result = pthread_cond_destroy(cond);
    if(result != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed cond_destroy!", (int)result);
    }
}

void SACond_signal(const char* file, size_t line, pthread_cond_t* cond)
{
    int result = pthread_cond_signal(cond);
    if(result != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed cond_signal!", (int)result);
    }
}

void SACond_wait(const char* file, size_t line, pthread_cond_t* cond, pthread_mutex_t* mutex)
{
    int result = pthread_cond_wait(cond, mutex);
    if(result != 0)
    {
        SASYS_LOG_ERROR_ERRNO_MANUAL(file, line, "Failed cond_wait!", (int)result);
    }
}

int SACond_timewait(const char* file, size_t line, pthread_cond_t* cond, pthread_mutex_t* mutex, const struct timespec* abstime)
{
    int result = pthread_cond_timedwait(cond, mutex, abstime);

    if(result == ETIMEDOUT || result == EAGAIN)
    {
        SASYS_LOG_ERROR_MANUAL(file, line, "It's timeouted.");
    }else if(result != 0){
        SASYS_LOG_ERROR_MANUAL(file, line, "Failed cond_timewait!");
    }
    return result;
}