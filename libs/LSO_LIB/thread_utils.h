//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_THREAD_UTILS_H
#define LSO_LIB_THREAD_UTILS_H

#pragma once
#include <pthread.h>
#include <stdlib.h>

#define mutex_lock(mutex)       pthread_mutex_lock(&(mutex))
#define mutex_unlock(mutex)     pthread_mutex_unlock(&(mutex))
#define mutex_init(mutex)       pthread_mutex_init(&(mutex), NULL)
#define mutex_destroy(mutex)    pthread_mutex_destroy(&(mutex))

#define cond_wait(cond, mutex)  pthread_cond_wait(&(cond), &(mutex))
#define cond_signal(cond)       pthread_cond_signal(&(cond))
#define cond_broadcast(cond)    pthread_cond_broadcast(&(cond))
#define cond_init(cond)         pthread_cond_init(&(cond), NULL)
#define cond_destroy(cond)      pthread_cond_destroy(&(cond))

int mutex_timed_lock(pthread_mutex_t *mutex, size_t nsec);
int condvar_timed_wait(pthread_cond_t *condvar,
                       pthread_mutex_t *mutex, size_t nsec);

int tu_create_thread_detached(void *(*proc)(void *), void *arg);
pthread_t tu_create_thread(void *(*proc)(void *), void *arg);


#endif //LSO_LIB_THREAD_UTILS_H
