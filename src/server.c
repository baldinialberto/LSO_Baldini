#include <server.h>


int main(int argc, char** argv)
{
	assert(sizeof(server_command_t) == 4);

	server_settings settings = parse_settings();
	server_stats stats = {0};
	server_infos infos = {0};
	
	DEBUG(print_server_settings(&settings));
	
	infos.nworkers = settings.nworkers;
	CHECK_BADVAL_PERROR_EXIT(
		infos.workers = calloc(infos.nworkers, sizeof(pthread_t)), 
		NULL, "main : calloc"
	);
	CHECK_BADVAL_PERROR_EXIT(
		infos.workers_clients = calloc(infos.nworkers, sizeof(int)), 
		NULL, "main : calloc"
	);
	infos.server_socket_fd = create_server_socket(&settings);

	pthread_t signalhandler_thread;
	pthread_create(&signalhandler_thread, NULL, &server_signalhandler, (void *)&infos);

	pthread_t dispatcher_thread;
	pthread_create(&dispatcher_thread, NULL, &server_dispatcher, (void *)&infos);

	pthread_join(dispatcher_thread, NULL);

	return EXIT_SUCCESS;
}

void *server_dispatcher(void *infos)
{
	server_infos* s_infos = (server_infos*) infos;

	DEBUG(puts("Dispatcher"));

	pthread_t *threads = malloc(s_infos->nworkers * sizeof(pthread_t));
	client_queue* client_queues = calloc(s_infos->nworkers, sizeof(client_queue));
	
	for (int i = 0; i < s_infos->nworkers; i++)
	{
		pthread_mutex_init(&(client_queues[i].mutex), NULL);
		pthread_cond_init(&(client_queues[i].empty), NULL);
	}

	for (int i = 0; i < s_infos->nworkers; i++)
		pthread_create(threads + i, NULL,
			&server_worker, client_queues + i);
	

	


	for (int i = 0; i < s_infos->nworkers; i++)
		pthread_join(threads[i], NULL);

	pthread_exit((void *)0);
}

void *server_worker(void *clients)
{
	DEBUG(puts("Worker"));
	pthread_exit((void *)0);
}

void *server_signalhandler(void *infos)
{
	server_infos* s_infos = (server_infos*) infos;

	int sig;
	sigset_t set;
	sigemptyset(&set);

	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGHUP);

	sigwait(&set, &sig);

	if (sig == SIGINT || sig == SIGQUIT)
	{
		s_infos->server_quit = 1;
	} else 
	{
		s_infos->server_hu = 1;
	}

	pthread_exit(NULL);
}