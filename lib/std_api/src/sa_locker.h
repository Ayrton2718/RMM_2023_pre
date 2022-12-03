#ifndef STD_API_SRC_SA_LOCKER_H
#define STD_API_SRC_SA_LOCKER_H

#include "system/sa_type.h"

#include <pthread.h>
#include "sa_bool.h"

#ifdef __cplusplus
 extern "C" {
#endif


#ifdef SATYPE_CHECKER_ON

void SALocker_init(const char* file, size_t line, pthread_mutex_t* mutex, pthread_mutexattr_t* attr);
void SALocker_destroy(const char* file, size_t line, pthread_mutex_t* mutex);
void SALocker_lock(const char* file, size_t line, pthread_mutex_t* mutex);
void SALocker_unlock(const char* file, size_t line, pthread_mutex_t* mutex);

void SACond_init(const char* file, size_t line, pthread_cond_t* cond, pthread_condattr_t* attr);
void SACond_destroy(const char* file, size_t line, pthread_cond_t* cond);
void SACond_signal(const char* file, size_t line, pthread_cond_t* cond);
void SACond_wait(const char* file, size_t line, pthread_cond_t* cond, pthread_mutex_t* mutex);
int SACond_timewait(const char* file, size_t line, pthread_cond_t* cond, pthread_mutex_t* mutex, const struct timespec* abstime);

#define SALOCKER_INIT(mutex, attr)  SALocker_init(__FILE__, __LINE__, mutex, attr)
#define SALOCKER_DESTROY(mutex)     SALocker_destroy(__FILE__, __LINE__, mutex)
#define SALOCKER_LOCK(mutex)        SALocker_lock(__FILE__, __LINE__, mutex)
#define SALOCKER_UNLOCK(mutex)      SALocker_unlock(__FILE__, __LINE__, mutex)

#define SACOND_INIT(cond, attr)                 SACond_init(__FILE__, __LINE__, cond, attr)
#define SACOND_DESTROY(cond)                    SACond_destroy(__FILE__, __LINE__, cond)
#define SACOND_SIGNAL(cond)                     SACond_signal(__FILE__, __LINE__, cond)
#define SACOND_WAIT(cond, mutex)                SACond_wait(__FILE__, __LINE__, cond, mutex)
#define SACOND_TIMEWAIT(cond, mutex, abstime)   SACond_timewait(__FILE__, __LINE__, cond, mutex, abstime)

#else

#define SALOCKER_INIT(mutex, attr)  pthread_mutex_init(mutex, attr)
#define SALOCKER_DESTROY(mutex)     pthread_mutex_destroy(mutex)
#define SALOCKER_LOCK(mutex)        pthread_mutex_lock(mutex)
#define SALOCKER_UNLOCK(mutex)      pthread_mutex_unlock(mutex)

#define SACOND_INIT(cond, attr)                 pthread_cond_init(__FILE__, __LINE__, cond, attr)
#define SACOND_DESTROY(cond)                    pthread_cond_destroy(__FILE__, __LINE__, cond)
#define SACOND_SIGNAL(cond)                     pthread_cond_signal(__FILE__, __LINE__, cond)
#define SACOND_WAIT(cond, mutex)                pthread_cond_wait(__FILE__, __LINE__, cond, mutex)
#define SACOND_TIMEWAIT(cond, mutex, abstime)   pthread_cond_timedwait(__FILE__, __LINE__, cond, mutex, abstime)

#endif /*CSMEM_DEBUG_ON*/


#ifdef __cplusplus
}
#endif

#endif /* STD_API_SRC_SA_LOCKER_H */