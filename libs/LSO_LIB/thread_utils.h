//
// Created by Alberto Baldini on 31/05/21.
//

#ifndef LSO_LIB_THREAD_UTILS_H
#define LSO_LIB_THREAD_UTILS_H

#pragma once
#include <pthread.h>
#include <stdlib.h>
#include "error_utils.h"

#define mutex_t                 pthread_mutex_t
#define mutex_lock(mutex)       if (pthread_mutex_lock(&(mutex))) {perror("pthread_mutex_lock"); errno = 0;}
#define mutex_trylock(mutex)    int trylock_res; if((trylock_res = pthread_mutex_trylock(&(mutex)))) {perror("pthread_mutex_trylock"); errno = 0;};
#define mutex_unlock(mutex)     if(pthread_mutex_unlock(&(mutex))) {perror("pthread_mutex_unlock"); errno = 0;}
#define mutex_init(mutex)       if(pthread_mutex_init(&(mutex), NULL)) {perror("pthread_mutex_init"); errno = 0;}
#define mutex_destroy(mutex)    if(pthread_mutex_destroy(&(mutex))) {perror("pthread_mutex_destroy"); errno = 0;}

#define cond_t                  pthread_cond_t
#define cond_wait(cond, mutex)  pthread_cond_wait(&(cond), &(mutex))
#define cond_signal(cond)       pthread_cond_signal(&(cond))
#define cond_broadcast(cond)    pthread_cond_broadcast(&(cond))
#define cond_init(cond)         pthread_cond_init(&(cond), NULL)
#define cond_destroy(cond)      pthread_cond_destroy(&(cond))

int mutex_timed_lock(pthread_mutex_t* mutex, size_t nsec);
int condvar_timed_wait(pthread_cond_t* condvar,
		pthread_mutex_t* mutex, size_t nsec);

int tu_create_thread_detached(void* (* proc)(void*), void* arg);
pthread_t tu_create_thread(void* (* proc)(void*), void* arg);

#endif //LSO_LIB_THREAD_UTILS_H
