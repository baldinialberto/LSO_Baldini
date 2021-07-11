//
// Created by Alberto Baldini on 31/05/21.
//

#include "thread_utils.h"
#include <stdio.h>

int mutex_timed_lock(pthread_mutex_t* mutex, size_t nsec)
{
	return 0;
}
int condvar_timed_wait(pthread_cond_t* condvar, pthread_mutex_t* mutex, size_t nsec)
{
	return 0;
}
int tu_create_thread_detached(void* (* proc)(void*), void* arg)
{
	if (proc==NULL) {
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
	if (proc==NULL) {
		fprintf(stdout, "tu_create_thread : wrong params\n");
		fflush(stdout);
		return (pthread_t)0;
	}
	pthread_t pt;
	pthread_create(&pt, NULL, proc, arg);
	return pt;
}
