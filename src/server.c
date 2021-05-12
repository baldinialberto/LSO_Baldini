#include <server.h>


int main(int argc, char** argv)
{
	assert(sizeof(server_command_t) == 4);

	server_settings settings = parse_settings();
	server_stats stats;
	server_infos infos;
	
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
		pthread_create(threads + i, NULL,
			&server_worker, client_queues + i);
	

	pthread_exit((void *)0);
}

void *server_worker(void *clients)
{
	pthread_exit((void *)0);
}