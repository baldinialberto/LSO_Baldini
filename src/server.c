#include <server.h>

int main(int argc, char** argv)
{
	assert(sizeof(server_command_t) == 4);

	static server_settings settings;
	static server_stats stats;
	static server_infos infos;
	static SFS_FS memory;
	settings = init_server_settings();
	stats = init_server_stats();
	infos = init_server_infos(&settings);
	sfs_init(&memory, settings.avaiableMemory, settings.maxFileCount);
	infos.memory = &memory;

	DEBUG(sfs_find(&memory, "/home/alberto/LSO_Baldini/src/lol.txt"));

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
	if (!infos->server_quit) infos->server_quit = 1;

	DEBUG(puts("Cleanup"));
	join_workers(infos);
	sq_free(sq);
}

void *server_worker(void *worker_arg)
{
	DEBUG(puts("Worker"));
	ignore_signals();

	struct _worker_arg *wa = (struct _worker_arg*) worker_arg;
	int request;
	struct timespec condtime;
	condtime.tv_sec = 0;
	condtime.tv_nsec = 0xFFFF;

	while (!wa->infos->server_quit)
	{
		pthread_mutex_lock(wa->infos->worker_locks + wa->worker_id);
		while ((wa->infos->workers_clients)[wa->worker_id] == NULL)
		{
			if (wa->infos->server_quit) 
			{
				free(worker_arg);
				pthread_exit(NULL);
			}
			pthread_cond_timedwait(
				wa->infos->worker_conds + wa->worker_id, 
				wa->infos->worker_locks + wa->worker_id, 
				&condtime
			);
		}
		read((wa->infos->workers_clients)[wa->worker_id], 
			&request, sizeof(int)
		);
		serve(request, 
			(wa->infos->workers_clients)[wa->worker_id], 
			wa->infos->memory
		);

		(wa->infos->workers_clients)[wa->worker_id] = NULL;
	}

	free(worker_arg);

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
	struct _worker_arg *arg;
	for (int i = 0; i < infos->nworkers; i++)
	{
		arg = (struct _worker_arg *)malloc(sizeof(struct _worker_arg));
		arg->worker_id = i;
		arg->infos = infos;
		(infos->workers)[i] = thread_spawn(
			&server_worker, 
			(void *)(arg)
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

int serve(int request, int client_socket, SFS_FS* memory)
{
	int (*server_ops[9])(int, int, SFS_FS*) = {
		server_openFile, 
		server_closeFile, 
		server_readFile, 
		server_writeFile,
		server_appendToFile,
		server_removeFile,
		server_lockFile,
		server_unlockFile/*,
		server_closeConnection*/
	};

	if (request & OP_MASK > 8/*9*/) return -1;

	return server_ops[request & OP_MASK](request, client_socket, memory);
}
/*
int server_closeConnection(int client_socket)
{
	DEBUG(puts("server_closeConnection"));
	return 0;
}
*/
int server_openFile(int request, int client_socket, SFS_FS* memory)
{
	DEBUG(puts("server_openFile"));
	unsigned int namelen = request >> MESSG_SHIFT;
	char *filename;
	int response = 0;

	CHECK_BADVAL_PERROR_EXIT(
		filename = calloc(namelen + 1, sizeof(char)), 
		NULL, "server_openFile : calloc" 
	);
	CHECK_BADVAL_CALL_RETURN(
		read(client_socket, filename, namelen * sizeof(char)), 
		-1, free(filename), S_NOREAD
	);

	retptr file = sfs_find(memory, filename);
	if (request & O_CREATE_FLAG && file.errorcodes == 0)
	{
		// create an already existent file
		response = ALREADY_FLAG;
		CHECK_BADVAL_CALL_RETURN(
			write(client_socket, &response, sizeof(int)), 
			-1, free(filename), S_NOWRITE
		);
		free(filename);
		return 0;
	}
	if (!(request & O_CREATE_FLAG) && file.errorcodes == SFS_FILENOTFOUND)
	{
		// not create and not existent file
		response = FNF_FLAG;
		CHECK_BADVAL_CALL_RETURN(
			write(client_socket, &response, sizeof(int)), 
			-1, free(filename), S_NOWRITE
		);
		free(filename);
		return 0;
	}
	if (request & O_CREATE_FLAG)
	{
		file = sfs_create(memory, filename, NULL, 0);
		if (!file.errorcodes)
			sfs_open(memory, filename, 'w');
		if (file.errorcodes & SFS_FILELOCKED)
		{
			response = O_LOCK_FLAG;
			if (write(client_socket, &response, sizeof(int)) != 0)
			{
				sfs_unlock((sfs_fd *)file.ptr, client_socket);
				sfs_close(memory, (sfs_fd *)file.ptr);
				sfs_remove(memory, filename);
				return S_NOWRITE;
			}
			return 0;
		}
	}
	else 
	{
		file = sfs_open(memory, filename, 'w');
		if (file.errorcodes & SFS_FILELOCKED)
		{
			response = O_LOCK_FLAG;
			if (write(client_socket, &response, sizeof(int)) != 0)
			{
				free(filename);
				return S_NOWRITE;
			}
			return 0;
		}
	}
	if (request & O_LOCK_FLAG)
	{
		sfs_lock((sfs_fd *)file.ptr, client_socket);
	}

	return 0;
}
int server_readFile(int request, int client_socket, SFS_FS* memory)
{
	DEBUG(puts("server_readFile"));
	unsigned int namelen = request >> MESSG_SHIFT;
	char filename[namelen + 1];
	server_command_t response = 0;

	CHECK_BADVAL_RETURN(
		read(client_socket, filename, namelen * sizeof(char)), 
		-1, S_NOREAD
	);

	retptr file = sfs_find(memory, filename);

	if (file.errorcodes == SFS_FILENOTFOUND)
	{
		response = FNF_FLAG;
		CHECK_BADVAL_RETURN(
			write(client_socket, &response, sizeof(int)), 
			-1, S_NOWRITE
		);
	}
	if (sfs_islocked((sfs_fd *)file.ptr, client_socket) == SFS_FILELOCKED)
	{
		response = LOCKED_FLAG;
		CHECK_BADVAL_RETURN(
			write(client_socket, &response, sizeof(int)), 
			-1, S_NOWRITE
		);
	}

	response = ((sfs_fd *)file.ptr)->file->datalen << MESSG_SHIFT;

	CHECK_BADVAL_RETURN(
		write(client_socket, &response, sizeof(int)), 
		-1, S_NOWRITE
	);

	CHECK_BADVAL_RETURN(
		write(client_socket, ((sfs_fd *)file.ptr)->file->data, ((sfs_fd *)file.ptr)->file->datalen), 
		-1, S_NOWRITE
	);

	return 0;
}
int server_writeFile(int request, int client_socket, SFS_FS* memory)
{
	DEBUG(puts("server_writeFile"));
	return 0;
}
int server_appendToFile(int request, int client_socket, SFS_FS* memory)
{
	DEBUG(puts("server_appendToFile"));
	return 0;
}
int server_lockFile(int request, int client_socket, SFS_FS* memory)
{
	DEBUG(puts("server_lockFile"));
	return 0;
}
int server_unlockFile(int request, int client_socket, SFS_FS* memory)
{
	DEBUG(puts("server_unlockFile"));
	return 0;
}
int server_closeFile(int request, int client_socket, SFS_FS* memory)
{
	DEBUG(puts("server_closeFile"));
	return 0;
}
int server_removeFile(int request, int client_socket, SFS_FS* memory)
{
	DEBUG(puts("server_removeFile"));
	return 0;
}