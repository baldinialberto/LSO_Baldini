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

	thread_spawn_detached(&server_signalhandler, (void *) &infos);
	thread_spawn_detached(&server_dispatcher, (void *) &infos);

	while (!infos.server_quit && !infos.server_hu);

	free(infos.workers);
	free(infos.workers_clients);

	return EXIT_SUCCESS;
}

void *server_dispatcher(void *infos)
{
	server_infos* s_infos = (server_infos*) infos;

	DEBUG(puts("Dispatcher"));

	s_infos->workers = calloc(s_infos->nworkers, sizeof(pthread_t));
	client_queue* cq = calloc(1, sizeof(client_queue));
	
	
	
	while (!s_infos->server_hu && !s_infos->server_quit)
	{

	}
	
	DEBUG(puts("Cleanup"));
	cq_free(cq);

	pthread_exit(NULL);
}

void *server_worker(void *client)
{
	DEBUG(puts("Worker"));
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
		printf("server_signalhandler received signal %d\n", sig);
	);

	s_infos->server_quit = 1;
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
