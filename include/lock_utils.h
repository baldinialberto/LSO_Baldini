#ifndef _LOCK_UTILS_H
#define _LOCK_UTILS_H

#include <pthread.h>

#define MUTEX_LOCK(LOCK)        pthread_mutex_lock(&(LOCK))
#define MUTEX_TRYLOCK(LOCK)     pthread_mutex_trylock(&(LOCK))
#define MUTEX_TIMED_LOCK(LOCK)  MUTEX_TRYLOCK(LOCK)
#define MUTEX_UNLOCK(LOCK)      pthread_mutex_unlock(&(LOCK))

typedef pthread_mutex_t mutex_t;


#endif
