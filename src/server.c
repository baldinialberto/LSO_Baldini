#include <server.h>

int main(int argc, char** argv)
{
	assert(sizeof(server_command_t) == 4);

	server_settings settings = init_server_settings();
	server_stats stats = init_server_stats();
	server_infos infos = init_server_infos(&settings);
	
	DEBUG(print_server_settings(&settings));
	
	thread_spawn_detached(&server_signalhandler, (void *) &infos);
	
	ignore_signals();

	server_dispatcher(&infos);

	close(infos.server_socket_fd);
	unlink(settings.socket_name);
	free_server_infos(&infos);

	return EXIT_SUCCESS;
}

void server_dispatcher(server_infos *infos)
{
	DEBUG(puts("Dispatcher"));
	socket_queue* sq = calloc(1, sizeof(socket_queue));
	
	spawn_workers(infos);
	
	sq_push(sq, infos->server_socket_fd, SQ_SSOCKET_FLAG);
	sq_update_arr(sq);

	int poll_ready = 0;

	while (pthread_mutex_lock(&(sq->mutex)), 
		!(infos->server_hu && !sq_has_clients(sq)) && !infos->server_quit)
	{
		if (infos->server_hu) sq_remove(sq, infos->server_socket_fd);
		DEBUG(puts("ServerWaiting"));
		sq_update_arr(sq);
		poll_ready = poll(sq->pollarr, sq->nsockets, 1000);

		if (poll_ready)
		{
			if ((poll_ready == infos->server_socket_fd))
			{
				sq_push(sq, 
					accept(infos->server_socket_fd, NULL, 0), 
					SQ_SSOCKET_FLAG
				);
			} else 
			{
				assign_client(infos, poll_ready);
			}
		}
		pthread_mutex_unlock(&(sq->mutex));
	}
	pthread_mutex_unlock(&(sq->mutex)); // unlock mutex at server_hu

	DEBUG(puts("Cleanup"));
	join_workers(infos);
	sq_free(sq);
}

void *server_worker(void *client)
{
	DEBUG(puts("Worker"));

	ignore_signals();

	pthread_exit(NULL);
}

void *server_signalhandler(void *infos)
{
	DEBUG(puts("Signal_handler"));

	server_infos* s_infos = (server_infos*) infos;

	int sig = 0;
	sigset_t set;
	sigemptyset(&set);

	if ( sigaddset(&set, SIGINT) 
		|| sigaddset(&set, SIGQUIT) 
		|| sigaddset(&set, SIGHUP) )
		perror("server_signalhandler : sigaddset");

	if (sigwait(&set, &sig))
		perror("server_signalhandler : sigwait");

	DEBUG(
		printf("\nserver_signalhandler received signal %d\n", sig);
	);

	if (sig == SIGINT || sig == SIGQUIT) 
		s_infos->server_quit = 1;
	else 
		s_infos->server_hu = 1;

	pthread_exit(NULL);
}

int spawn_workers(server_infos* infos)
{
	for (int i = 0; i < infos->nworkers; i++)
	{
		(infos->workers)[i] = thread_spawn(
			&server_worker, 
			(void *)(infos->workers_clients + i)
			);
	}
	return 0;
}

int join_workers(server_infos* infos)
{
	for (int i = 0; i < infos->nworkers; i++)
	{
		pthread_join((infos->workers)[i], NULL);
	}
	return 0;
}

int assign_client(server_infos *infos, int client)
{
	if (infos == NULL || client < 0) return -1;

	for (int i = 0;;i = ((i + 1) % infos->nworkers))
	{
		if (!pthread_mutex_trylock(infos->worker_locks + i))
		{
			(infos->workers_clients)[i] = client;
			pthread_cond_signal(infos->worker_conds + i);
			pthread_mutex_unlock(infos->worker_locks + i);
			break;
		}
	}

	return 0;
}

int ignore_signals()
{
	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGINT);

    return pthread_sigmask(SIG_BLOCK, &set, NULL);
}