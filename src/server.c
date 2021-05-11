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

	

	return EXIT_SUCCESS;
}

void *server_dispatcher(void *infos)
{
	server_infos* s_infos = (server_infos*) infos;

	int *client_socket;
	pthread_attr_t tattr;
	pthread_t tid;

	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);

	while (1)
	{
		client_socket = malloc(sizeof(int));
		*client_socket = accept(s_infos->server_socket_fd, NULL, 0);
		//pthread_create(&tid, &tattr, to_upper, (void *)client_socket);
		DEBUG(puts("SERVER ACCEPTED");)
	}

	pthread_exit((void *)0);
}