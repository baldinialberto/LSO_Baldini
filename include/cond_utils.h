#ifndef _CONDVARS_H
#define _CONDVARS_H

#include <pthread.h>

#define COND_WAIT(COND, LOCK)   pthread_cond_wait(&(COND), &(LOCK))
#define COND_SIGNAL(COND)       pthread_cond_signal(&(COND))

typedef pthread_cond_t cond_t;



#endif