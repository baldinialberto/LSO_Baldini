//
// Created by Alberto Baldini on 31/05/21.
//

#include "thread_utils.h"

int mutex_timed_lock(pthread_mutex_t *mutex, size_t nsec)
{
    return 0;
}
int condvar_timed_wait(pthread_cond_t *condvar, pthread_mutex_t *mutex, size_t nsec)
{
    return 0;
}
int tu_create_thread_detached(void *(*proc)(void *), void *arg)
{
    return 0;
}
pthread_t tu_create_thread(void *(*proc)(void *), void *arg)
{
    return (pthread_t) 0;
}
