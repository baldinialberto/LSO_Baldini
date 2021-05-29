#include "../include/server.h"

int main(int argc, char** argv)
{
	assert(sizeof(server_command_t) == 4);

	static server_settings settings;
	//static server_stats stats;
	static server_infos infos;
	static SFS_FS memory;
	settings = init_server_settings();
	//stats = init_server_stats();
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
	infos->sq = calloc(1, sizeof(socket_queue));
	
	spawn_workers(infos);
	
	sq_push(infos->sq, infos->server_socket_fd, SQ_SSOCKET_FLAG);
	sq_update_arr(infos->sq);

	int poll_ready = 0;

	while (pthread_mutex_lock(&(infos->sq->mutex)), 
		!(infos->server_hu && !sq_has_clients(infos->sq)) && !infos->server_quit)
	{
		if (infos->server_hu) sq_remove(infos->sq, infos->server_socket_fd);
		DEBUG(puts("ServerWaiting"));
		poll_ready = poll(infos->sq->pollarr, infos->sq->nsockets, 1000);

		for (int i = 0; poll_ready && i < infos->sq->nsockets; i++)
		{
			if (infos->sq->pollarr[i].revents & POLLIN)
			{	
				if (infos->sq->pollarr[i].fd != infos->server_socket_fd) 
				{
					assign_client(infos, infos->sq->pollarr[i].fd);
				}
				else
				{
					sq_push(infos->sq, 
					accept(infos->server_socket_fd, NULL, 0), 
					SQ_CSOCKET_FLAG
					);
				} 
			}
			if (infos->sq->pollarr[i].revents & POLLHUP)
			{
				close(infos->sq->pollarr[i].fd);
				sq_remove(infos->sq, infos->sq->pollarr[i].fd);
			}
			infos->sq->pollarr[i].revents &= 0;
		}

		pthread_mutex_unlock(&(infos->sq->mutex));
	}

	pthread_mutex_unlock(&(infos->sq->mutex)); // unlock mutex at server_hu
	if (!infos->server_quit) infos->server_quit = 1;

	DEBUG(puts("Cleanup"));
	join_workers(infos);
	sq_free(infos->sq);
}

void *server_worker(void *worker_arg)
{
	DEBUG(puts("Worker"));
	ignore_signals();

	struct _worker_arg *wa = (struct _worker_arg*) worker_arg;
	int request = -1;
	struct timespec condtime;
	condtime.tv_sec = 0;
	condtime.tv_nsec = 0xFFFF;

	while (!wa->infos->server_quit)
	{
		pthread_mutex_lock(wa->infos->worker_locks + wa->worker_id);
		while ((wa->infos->workers_clients)[wa->worker_id] == 0)
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
		request = 0;
		read((wa->infos->workers_clients)[wa->worker_id], 
			&request, sizeof(int)
		);
		printf("received request %X from %d\n", request, (wa->infos->workers_clients)[wa->worker_id]);
		if (request & OP_MASK)
			serve(request, 
				(wa->infos->workers_clients)[wa->worker_id], 
				wa->infos->memory
			);

		(wa->infos->workers_clients)[wa->worker_id] = 0;
		pthread_mutex_unlock(wa->infos->worker_locks + wa->worker_id);
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
	int (*server_ops[8])(int, int, SFS_FS*) = {
		server_openFile, 
		server_closeFile, 
		server_readFile, 
		server_writeFile,
		server_appendToFile,
		server_removeFile,
		server_lockFile,
		server_unlockFile
	};

	if ((request & OP_MASK) > 8) return -1;

	return server_ops[(request & OP_MASK) - 1](request, client_socket, memory);
}

int server_closeConnection(int client_socket, server_infos *infos)
{
	DEBUG(puts("server_closeConnection"));
	int response = 0;
	write(client_socket, &response, sizeof(int));
	sq_remove(infos->sq, client_socket);
	return 0;
}

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
	filename[namelen] = (char) 0;
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
	unsigned int namelen = request >> MESSG_SHIFT;
	char filename[namelen + 1];
	filename[namelen] = (char) 0;
	CHECK_BADVAL_RETURN(
		read(client_socket, &filename, namelen), 
		-1, -1
	);
	char *data;
	unsigned int datalen;
	CHECK_BADVAL_RETURN(
		read(client_socket, &datalen, sizeof(server_command_t)), 
		-1, -1
	);
	CHECK_BADVAL_PERROR_EXIT
	(
		data = (char *)calloc(datalen, sizeof(char)), 
		NULL, "server_writeFile : calloc"
	);

	retptr res;
	if (memory->maxSize - memory->currentSize < datalen)
	{
		res = sfs_evict(memory, datalen);
		if (res.errorcodes) return -1;
	}
	CHECK_BADVAL_RETURN(
		read(client_socket, data, datalen), 
		-1, -1
	);


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
server_settings init_server_settings()
{
	return parse_settings();
}

server_stats init_server_stats(server_settings *setts)
{
	server_stats stats = {0};
	return stats;
}

server_infos init_server_infos(server_settings *setts)
{
	server_infos infos;

	infos.server_socket_fd = create_server_socket(setts);
	infos.nworkers = setts->nworkers;
	CHECK_BADVAL_PERROR_EXIT(
		infos.workers = calloc(infos.nworkers, sizeof(pthread_t)), 
		NULL, "init_server_infos : calloc"
	);
	CHECK_BADVAL_PERROR_EXIT(
		infos.workers_clients = calloc(infos.nworkers, sizeof(int)), 
		NULL, "init_server_infos : calloc"
	);
	CHECK_BADVAL_PERROR_EXIT(
		infos.worker_locks = calloc(infos.nworkers, sizeof(mutex_t)), 
		NULL, "init_server_infos : calloc"
	);
	CHECK_BADVAL_PERROR_EXIT(
		infos.worker_conds = calloc(infos.nworkers, sizeof(cond_t)), 
		NULL, "init_server_infos : calloc"
	);
	for (int i = 0; i < infos.nworkers; i++)
	{
		pthread_mutex_init(infos.worker_locks + i, NULL);
		pthread_cond_init(infos.worker_conds + i, NULL);
	}
	infos.server_quit = (infos.server_hu = 0);

	return infos;
}

int free_server_infos(server_infos *infos)
{
	if (infos == NULL)
		return -1;

	if (infos->workers != NULL) free(infos->workers);
	if (infos->workers_clients != NULL) free(infos->workers_clients);
	for (int i = 0; i < infos->nworkers; i++)
	{ 
		pthread_cond_destroy(infos->worker_conds + i);
		pthread_mutex_destroy(infos->worker_locks + i);
	}
	if (infos->worker_conds != NULL) free(infos->worker_conds);
	if (infos->worker_locks != NULL) free(infos->worker_locks);
	if (infos->memory->filetable != NULL) free(infos->memory->filetable);

	return 0;
}

void print_server_settings(server_settings *setts)
{
	fprintf(
		stdout,
		"NWORKERS = %d\n\
CAPACITY = %dMB\n\
FILECAPACITY = %d files\n\
SOCKET_NAME = %s\n\
LOGFILE_NAME = %s\n", 
		setts->nworkers, 
		setts->avaiableMemory, 
		setts->maxFileCount, 
		setts->socket_name,
		setts->logfile_name
	);
	fflush(stdout);
}

server_settings parse_settings()
{
	server_settings settings = {0};

	FILE* configFile = NULL;
	CHECK_BADVAL_PERROR_RETURN(
		(configFile = fopen(SERVER_CONFIGFILE_PATH, "r")), 
		NULL, "fopen", settings
	);

	//size_t fstream_pos = 0;
	char *tempRead = NULL;

	while (!feof(configFile))
	{
		switch ((char)fgetc(configFile))
		{
		case '#': // comment
			f_skipline(configFile);
			break;
		case '\n': // emptyline
			break;
		default:
			CHECK_BADVAL_PERROR_EXIT(
				fseek(configFile, -1, SEEK_CUR),
				-1, "parse_settings : fseek"
			)
			if (f_readword(configFile, &tempRead) != 0)
			{
				if (tempRead != NULL) free(tempRead);
				fclose(configFile);
				CHECK_ERRNO_EXIT(-1, "parse_settings : fclose");
				return (server_settings) {0};
			}
			get_setting(&tempRead, configFile, &settings);
			f_skipline(configFile);
			break;
		}
		if (tempRead != NULL)
		{
			free(tempRead);
			tempRead = NULL;
		}
	}
	fclose(configFile);
	CHECK_ERRNO_EXIT(-1, "parse_settings : fclose");
	return settings;
}
int write_settings(server_settings* setts)
{
	return 0;
}

int write_log(const char* op, server_settings *setts)
{
	FILE* fOut;
	CHECK_BADVAL_PERROR_RETURN(
		fOut = fopen(setts->logfile_name, "a"), 
		NULL, "write_log : fopen", -1
	);

	time_t currTime;
	time(&currTime);
	char* currTimeString = ctime(&currTime); //The return value points to a statically allocated string
	if (errno) {
		perror("write_log : ctime");
		errno = 0;
		fclose(fOut);
		return -1;
	}
	currTimeString[strlen(currTimeString) - 1] = '\0';
	if (fprintf(fOut, "%s : %s\n", currTimeString, op) <= 0)
	{
		perror("write_log : fprintf");
		errno = 0;
		fclose(fOut);
		return -1;
	}

	fclose(fOut);

	return 0;
}

void get_setting(char** str, FILE *fstream, server_settings *setts)
{
	int int_sett = 0;

	fscanf(fstream, "%*[ ,;=\n\r]");
	if (feof(fstream)) return;

	if (!strcmp(*str, CONFIG_NWORKERS)) 
	{
		if (fscanf(fstream, "%d", &int_sett) != 1) return;
		setts->nworkers = int_sett;
	} 
	else if (!strcmp(*str, CONFIG_CAPACITY))
	{
		if (fscanf(fstream, "%d", &int_sett) != 1) return;
		setts->avaiableMemory = int_sett;
	}
	else if (!strcmp(*str, CONFIG_FILECAPACITY))
	{
		if (fscanf(fstream, "%d", &int_sett) != 1) return;
		setts->maxFileCount = int_sett;
	}
	else if (!strcmp(*str, CONFIG_SOCKET_NAME))
	{
		fscanf(fstream, "%*[^ ,;=\n\r]%n", &int_sett);
		fseek(fstream, -int_sett, SEEK_CUR);
		CHECK_ERRNO_EXIT(-1, "fseek : get_settings");

		if (feof(fstream)) return;
		CHECK_BADVAL_PERROR_EXIT(
			fread(&(setts->socket_name),
			sizeof(char), 
			int_sett < 64 ? int_sett : 63, 
			fstream), 0, 
			"fread : get_settings"
		);
		
	}
	else if (!strcmp(*str, CONFIG_LOGFILE_NAME))
	{
		fscanf(fstream, "%*[^ ,;=\n\r]%n", &int_sett);
		fseek(fstream, -int_sett, SEEK_CUR);
		CHECK_ERRNO_EXIT(-1, "fseek : get_settings");

		if (feof(fstream)) return;
		CHECK_BADVAL_PERROR_EXIT(
			fread(&(setts->logfile_name),
			sizeof(char), 
			int_sett < 64 ? int_sett : 63, 
			fstream), 0, 
			"fread : get_settings"
		);
		
	}
	else {
		puts("SettingFormat not recongnized");
	}
}

int create_server_socket(server_settings *setts)
{
	DEBUG(unlink(setts->socket_name));

	int socket_res;

	struct sockaddr_un sa;
	memset(&sa, 0, sizeof(struct sockaddr_un));

	strncpy(sa.sun_path, setts->socket_name, sizeof(sa.sun_path) - 1);

	sa.sun_family = AF_UNIX;
	CHECK_BADVAL_PERROR_EXIT(
		socket_res = socket(AF_UNIX, SOCK_STREAM, 0),
		-1, "create_server_socket : listen"
	);

	CHECK_BADVAL_PERROR_EXIT(
		bind(socket_res, (struct sockaddr *)&sa, sizeof(sa)),
		-1, "create_server_socket : bind"
	);
	CHECK_BADVAL_PERROR_EXIT(
		listen(socket_res, 256),
		-1, "create_server_socket : listen"
	);

	return socket_res;
}

int thread_spawn_detached(void *(*fun)(void*), void *arg)
{
	pthread_t thread;
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &thread_attr, fun, arg);

	return 0;
}

pthread_t thread_spawn(void *(*fun)(void*), void *arg)
{
	pthread_t thread;
	pthread_create(&thread, NULL, fun, arg);

	return thread;
}

int freeworker(server_infos *infos)
{
	return 0;
}