//
// Created by Alberto Baldini on 31/05/21.
//

#include "thread_utils.h"
#include "print_utils.h"
#include <stdio.h>

int mrsw_lock_reader(MRSW_mutex *mutex)
{
    if (mutex == NULL)
    {
        pu_print_pos(stderr, "param mutex is NULL");
        return -1;
    }
    mutex_lock(mutex->m);
    while(mutex->waiting_writers || mutex->writing)
    {
        cond_wait(mutex->c, mutex->m);
    }
    mutex->active_readers++;
    mutex_unlock(mutex->m);
    return 0;
}

int mrsw_unlock_reader(MRSW_mutex *mutex)
{
    if (mutex == NULL)
    {
        pu_print_pos(stderr, "param mutex is NULL");
        return -1;
    }
    mutex_lock(mutex->m);
    mutex->active_readers--;
    if (mutex->active_readers==0)
    {
        cond_broadcast(mutex->c);
    }
    mutex_unlock(mutex->m);
    return 0;
}

int mrsw_lock_writer(MRSW_mutex *mutex)
{
    if (mutex == NULL)
    {
        pu_print_pos(stderr, "param mutex is NULL");
        return -1;
    }
    mutex_lock(mutex->m);
    mutex->waiting_writers++;
    while(mutex->active_readers || mutex->writing)
    {
        cond_wait(mutex->c, mutex->m);
    }
    mutex->waiting_writers--;
    mutex->writing = 1;
    mutex_unlock(mutex->m);
    return 0;
}

int mrsw_unlock_writer(MRSW_mutex *mutex)
{
    if (mutex == NULL)
    {
        pu_print_pos(stderr, "param mutex is NULL");
        return -1;
    }
    mutex_lock(mutex->m);
    mutex->writing = 0;
    cond_broadcast(mutex->c);
    mutex_unlock(mutex->m);
    return 0;
}


int condvar_timed_wait(pthread_cond_t* condvar, pthread_mutex_t* mutex, size_t nsec)
{
	static struct timespec time;
	time.tv_nsec = (long)nsec;
	return pthread_cond_timedwait(condvar, mutex, &time);
}
int tu_create_thread_detached(void* (* proc)(void*), void* arg)
{
	if (proc == NULL)
	{
		fprintf(stdout, "tu_create_thread_detached : wrong params\n");
		fflush(stdout);
		return -1;
	}
	pthread_t pt;
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	pthread_create(&pt, &tattr, proc, arg);
	pthread_attr_destroy(&tattr);

	return 0;
}
pthread_t tu_create_thread(void* (* proc)(void*), void* arg)
{
	if (proc == NULL)
	{
		fprintf(stdout, "tu_create_thread : wrong params\n");
		fflush(stdout);
		return (pthread_t)0;
	}
	pthread_t pt;
	pthread_create(&pt, NULL, proc, arg);
	return pt;
}
