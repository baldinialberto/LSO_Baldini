#include "../include/server.h"

int main(int argc, char** argv)
{
	static server_settings settings;
	//static server_stats stats;
	static server_infos infos;
	settings = init_server_settings();
	//stats = init_server_stats();
	infos = init_server_infos(&settings);


	print_server_settings(&settings);
	
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
	
	spawn_workers(infos);
	
	pu_add(&(infos->pollarr), infos->server_socket_fd, POLL_IN);

	int poll_ready = 0;

	while (!(infos->server_hu && pu_isempty(&(infos->pollarr))) && !infos->server_quit)
	{
		if (infos->server_hu) pu_remove(&(infos->pollarr), infos->server_socket_fd);
		DEBUG(puts("ServerWaiting"));
		poll_ready = poll(infos->pollarr.arr, infos->pollarr.len, 1000);

		for (int i = 0; poll_ready && i < infos->pollarr.len; i++)
		{
			if (infos->pollarr.arr[i].revents & POLLIN)
			{	
				if (infos->pollarr.arr[i].fd != infos->server_socket_fd) 
				{
					assign_client(infos, infos->pollarr.arr[i].fd);
				}
				else if (!pu_isfull(&(infos->pollarr)))
				{
					pu_add(&(infos->pollarr), 
						accept(infos->server_socket_fd, NULL, 0), 
						POLL_IN | POLL_HUP
					);
				} 
			}
			if (infos->pollarr.arr[i].revents & POLLHUP)
			{
				close(infos->pollarr.arr[i].fd);
				pu_remove(&(infos->pollarr), infos->pollarr.arr[i].fd);
			}
		}
	}
	if (!infos->server_quit) infos->server_quit = 1;

	DEBUG(puts("Cleanup"));
	join_workers(infos);
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
		if (request)
			serve(request, 
				(wa->infos->workers_clients)[wa->worker_id], 
				&(wa->infos->storage)
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
		printf("\nserver_signalhandler received signal %d\n", sig)
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
int serve(int request, int client_socket, u_file_storage* storage)
{
	if (storage == NULL)
	{
		fprintf(stderr, "serve : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	unsigned char op = request & SAPI_OPMASK;
	switch(op)
	{
		case SAPI_OPENFILE:
			return server_openFile(request, client_socket, storage);
		case SAPI_CLOSEFILE:
			return server_closeFile(request, client_socket, storage);
		case SAPI_READFILE:
			return server_readFile(request, client_socket, storage);
		case SAPI_READNFILES:
			return server_readNFiles(request, client_socket, storage);
		case SAPI_WRITEFILE:
			return server_writeFile(request, client_socket, storage);
		case SAPI_APPENDFILE:
			return server_appendToFile(request, client_socket, storage);
		case SAPI_LOCKFILE:
			return server_lockFile(request, client_socket, storage);
		case SAPI_UNLOCKFILE:
			return server_unlockFile(request, client_socket, storage);
		case SAPI_REMOVEFILE:
			return server_removeFile(request, client_socket, storage);
		default:
			fprintf(stderr, "serve : unrecognized operation\n");
			fflush(stderr);
			return -1;
	}
}
int server_openFile(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_openFile : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_openFile : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_openFile : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	s_message m;
	const size_t filename_len = (request >> SAPI_MSSLEN_SHFT) + 1;
	char filename[filename_len];
	memset(filename, 0, filename_len);
	unsigned char flags = request & SAPI_FLAGMASK;
	if (read(client_socket, filename, filename_len - 1) == -1)
	{
		perror("server_openFile : read");
		fprintf(stderr, "server_openFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	u_file_data *fdata = fu_getfile(storage, filename);
	if ((fdata != NULL && (flags & O_CREATE)) || (fdata == NULL && !(flags & O_CREATE)))
	{
		fprintf(stderr, "server_openFile : file %s in storage\n", 
			(flags & O_CREATE) ? "already" : "not");
		fflush(stderr);
		return -1;
	}
	if (fdata == NULL) // create and open new file
	{
		u_file file = fu_allocfile_empty(filename);
		file.data->client = client_socket;
		if (flags & O_LOCK) file.data->datainfo |= O_LOCK;
		if (!fu_storage_avFiles(storage))
		{
			fprintf(stderr, "server_openFile : storage is full of files\n");
			fflush(stderr);
			return -1;
		}
		if (fu_addfile(storage, file.data, file.path) == -1)
		{	
			fprintf(stderr, "server_openFile : fu_addfile returned an error\n");
			fflush(stderr);
			return -1;
		}
	}
	else // open existing file
	{
		pthread_mutex_lock(&(fdata->mutex));
		if (fdata->datainfo & O_LOCK)
		{
			fprintf(stderr, "server_openFile : %s is already locked\n", filename);
			fflush(stderr);
			pthread_mutex_unlock(&(fdata->mutex));
			return -1;
		}
		if (fdata->client != 0)
		{
			fprintf(stderr, 
				"server_openFile : %s is already opened by another client(%d)\n", 
				filename, fdata->client);
			fflush(stderr);
			pthread_mutex_unlock(&(fdata->mutex));
			return -1;
		}
		fdata->client = client_socket;
		if (flags & O_LOCK) fdata->datainfo |= O_LOCK;
		pthread_mutex_unlock(&(fdata->mutex));
	}
	m = SAPI_SUCCESS;
	if (write(client_socket, &m, sizeof(s_message)) == -1)
	{
		perror("server_openFile : write");
		fprintf(stderr, "server_openFile : write returned an error\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}
int server_readFile(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_readFile : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_readFile : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_readFile : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	const size_t filename_len = (request >> SAPI_MSSLEN_SHFT) + 1;
	char filename[filename_len];
	memset(filename, 0, filename_len);
	if (read(client_socket, filename, filename_len - 1) == -1)
	{
		perror("server_readFile : read");
		fprintf(stderr, "server_readFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	u_file_data *fdata = fu_getfile(storage, filename);
	if (fdata == NULL)
	{
		if (server_sendresponse(SAPI_FNF, client_socket) == -1)
		{
			fprintf(stderr, "server_readFile : server_sendresponse returned an error\n");
			fflush(stderr);
			return -1;
		}
		fprintf(stderr, "server_readFile : file %s not in storage\n", filename);
		fflush(stderr);
		return -1;
	}
	else 
	{
		pthread_mutex_lock(&(fdata->mutex));
		if (fdata->client)
		{
			if (server_sendresponse(SAPI_ALREADY, client_socket) == -1)
			{
				fprintf(stderr, "server_readFile : server_sendresponse returned an error\n");
				fflush(stderr);
			}
			pthread_mutex_unlock(&(fdata->mutex));
			return -1;
		}
		else if (fdata->datainfo & O_LOCK)
		{
			if (server_sendresponse(SAPI_LOCKED, client_socket) == -1)
			{
				fprintf(stderr, "server_readFile : server_sendresponse returned an error\n");
				fflush(stderr);
			}
			pthread_mutex_unlock(&(fdata->mutex));
			return -1;
		}
		else 
		{
			if (server_sendresponse(SAPI_SUCCESS, client_socket) == -1)
			{
				fprintf(stderr, "server_readFile : server_sendresponse returned an error\n");
				fflush(stderr);
				pthread_mutex_unlock(&(fdata->mutex));
				return -1;
			}
			if (server_senddata(fdata->data, fdata->datalen, client_socket) == -1)
			{
				fprintf(stderr, "server_readFile : server_senddata returned an error\n");
				fflush(stderr);
				pthread_mutex_unlock(&(fdata->mutex));
				return -1;
			}
		}	
		pthread_mutex_unlock(&(fdata->mutex));
	}
	return 0;
}
int server_readNFiles(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_readNFiles : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_readNFiles : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_readNFiles : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}
int server_writeFile(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_writeFile : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_writeFile : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_writeFile : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	const size_t filename_len = (request >> SAPI_MSSLEN_SHFT) + 1;
	char filename[filename_len];
	memset(filename, 0, filename_len);
	if (read(client_socket, &filename, filename_len - 1) == -1)
	{
		perror("server_writeFile : read");
		fprintf(stderr, "server_writeFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	u_file_data *file = fu_getfile(storage, filename);
	if (file == NULL)
	{
		if (server_sendresponse(SAPI_FNF, client_socket) == -1)
		{
			fprintf(stderr, "server_writeFile : server_sendresponse returned an error\n");
			fflush(stderr);
		}
		return -1;
	}
	size_t datalen = 0;
	if (read(client_socket, &datalen, sizeof(size_t)) == -1)
	{
		perror("server_writeFile : read");
		fprintf(stderr, "server_writeFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	if (fu_storage_avBytes(storage) < datalen)
	{
		if (server_sendresponse(SAPI_EVICT, client_socket))
		{
			fprintf(stderr, "server_writeFile : server_sendresponse returned an error\n");
			fflush(stderr);
			return -1;
		}
		int res;
		if ((res =  server_getresponse(client_socket)) == -1)
		{
			fprintf(stderr, "server_writeFile : server_getresponse returned an error\n");
			fflush(stderr);
			return -1;
		}
		u_list files_to_evict;
		u_list_node *currnode;
		server_evictlist(&files_to_evict, datalen);
		if (res)
		{
			u_file_data *filedata;
			lu_foreach(files_to_evict, currnode, 
				server_sendresponse(SAPI_SUCCESS, client_socket);
				server_senddata(currnode->data, strlen((char *)(currnode->data)) + 1, client_socket);
				filedata = fu_getfile(storage, currnode->data);
				if (filedata != NULL)
				{
					server_senddata(filedata->data, filedata->datalen, client_socket);
				}
			)
			server_sendresponse(SAPI_FAILURE, client_socket);
		}
		lu_foreach(files_to_evict, currnode, 
			fu_removefile(storage, currnode->data)
		)
		lu_free(&files_to_evict, mu_free);
	}
	char data[datalen];
	if (read(client_socket, data, datalen) == -1)
	{
		perror("server_writeFile : read");
		fprintf(stderr, "server_writeFile : read returned an error\n");
		fflush(stderr);
		if (server_sendresponse(SAPI_FAILURE, client_socket))
		{
			fprintf(stderr, "server_writeFile : server_sendresponse returned an error\n");
			fflush(stderr);
			return -1;
		}
		return -1;
	}
	pthread_mutex_lock(&(file->mutex));
	if ((file->datainfo & O_LOCK) && file->client == 0)
	{
		fprintf(stderr, "server_writeFile : file %s is already locked\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (file->client != client_socket)
	{
		fprintf(stderr, "server_writeFile : file %s is open by another client\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (fu_writefile(file, 0, data, datalen) == -1)
	{
		if (server_sendresponse(SAPI_FAILURE, client_socket))
		{
			fprintf(stderr, "server_writeFile : server_sendresponse returned an error\n");
			fflush(stderr);
		}
		fprintf(stderr, "server_writeFile : fu_allocfile_oncopy returned an error\n");
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	pthread_mutex_unlock(&(file->mutex));
	if (server_sendresponse(SAPI_SUCCESS, client_socket))
	{
		fprintf(stderr, "server_writeFile : server_sendresponse returned an error\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}
int server_appendToFile(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_appendToFile : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_appendToFile : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_appendToFile : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	const size_t filename_len = (request >> SAPI_MSSLEN_SHFT) + 1;
	char filename[filename_len];
	memset(filename, 0, filename_len);
	if (read(client_socket, &filename, filename_len - 1) == -1)
	{
		perror("server_appendToFile : read");
		fprintf(stderr, "server_appendToFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	u_file_data *file = fu_getfile(storage, filename);
	if (file == NULL)
	{
		if (server_sendresponse(SAPI_FNF, client_socket) == -1)
		{
			fprintf(stderr, "server_appendToFile : server_sendresponse returned an error\n");
			fflush(stderr);
		}
		return -1;
	}
	size_t datalen = 0;
	if (read(client_socket, &datalen, sizeof(size_t)) == -1)
	{
		perror("server_appendToFile : read");
		fprintf(stderr, "server_appendToFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	if (fu_storage_avBytes(storage) < datalen)
	{
		if (server_sendresponse(SAPI_EVICT, client_socket))
		{
			fprintf(stderr, "server_writeFile : server_sendresponse returned an error\n");
			fflush(stderr);
			return -1;
		}
		int res;
		if ((res =  server_getresponse(client_socket)) == -1)
		{
			fprintf(stderr, "server_writeFile : server_getresponse returned an error\n");
			fflush(stderr);
			return -1;
		}
		u_list files_to_evict;
		u_list_node *currnode;
		server_evictlist(&files_to_evict, datalen);
		if (res)
		{
			u_file_data *filedata;
			lu_foreach(files_to_evict, currnode, 
				server_sendresponse(SAPI_SUCCESS, client_socket);
				server_senddata(currnode->data, strlen((char *)(currnode->data)) + 1, client_socket);
				filedata = fu_getfile(storage, currnode->data);
				if (filedata != NULL)
				{
					server_senddata(filedata->data, filedata->datalen, client_socket);
				}
			)
			server_sendresponse(SAPI_FAILURE, client_socket);
		}
		lu_foreach(files_to_evict, currnode, 
			fu_removefile(storage, currnode->data)
		)
		lu_free(&files_to_evict, mu_free);
	}
	char data[datalen];
	if (read(client_socket, data, datalen) == -1)
	{
		perror("server_appendToFile : read");
		fprintf(stderr, "server_appendToFile : read returned an error\n");
		fflush(stderr);
		if (server_sendresponse(SAPI_FAILURE, client_socket))
		{
			fprintf(stderr, "server_appendToFile : server_sendresponse returned an error\n");
			fflush(stderr);
			return -1;
		}
		return -1;
	}
	pthread_mutex_lock(&(file->mutex));
	if ((file->datainfo & O_LOCK) && file->client == 0)
	{
		fprintf(stderr, "server_appendToFile : file %s is already locked\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (file->client != client_socket)
	{
		fprintf(stderr, "server_appendToFile : file %s is open by another client\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (fu_writefile(file, file->datalen, data, datalen) == -1)
	{
		if (server_sendresponse(SAPI_FAILURE, client_socket))
		{
			fprintf(stderr, "server_appendToFile : server_sendresponse returned an error\n");
			fflush(stderr);
		}
		fprintf(stderr, "server_appendToFile : fu_allocfile_oncopy returned an error\n");
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	pthread_mutex_unlock(&(file->mutex));
	if (server_sendresponse(SAPI_SUCCESS, client_socket))
	{
		fprintf(stderr, "server_appendToFile : server_sendresponse returned an error\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}
int server_lockFile(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_lockFile : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_lockFile : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_lockFile : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	const size_t filename_len = (request >> SAPI_MSSLEN_SHFT) + 1;
	char filename[filename_len];
	memset(filename, 0, filename_len);
	if (read(client_socket, &filename, filename_len - 1) == -1)
	{
		perror("server_lockFile : read");
		fprintf(stderr, "server_lockFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	u_file_data *file = fu_getfile(storage, filename);
	if (file == NULL)
	{
		if (server_sendresponse(SAPI_FNF, client_socket) == -1)
		{
			fprintf(stderr, "server_lockFile : server_sendresponse returned an error\n");
			fflush(stderr);
		}
		return -1;
	}
	pthread_mutex_lock(&(file->mutex));
	if ((file->datainfo & O_LOCK))
	{
		fprintf(stderr, "server_lockFile : file %s is already locked\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (file->client != client_socket)
	{
		fprintf(stderr, "server_lockFile : file %s is open by another client\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	file->datainfo |= O_LOCK;
	file->client = client_socket;
	pthread_mutex_unlock(&(file->mutex));
	return 0;
}
int server_unlockFile(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_unlockFile : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_unlockFile : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_unlockFile : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	const size_t filename_len = (request >> SAPI_MSSLEN_SHFT) + 1;
	char filename[filename_len];
	memset(filename, 0, filename_len);
	if (read(client_socket, &filename, filename_len - 1) == -1)
	{
		perror("server_unlockFile : read");
		fprintf(stderr, "server_unlockFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	u_file_data *file = fu_getfile(storage, filename);
	if (file == NULL)
	{
		if (server_sendresponse(SAPI_FNF, client_socket) == -1)
		{
			fprintf(stderr, "server_unlockFile : server_sendresponse returned an error\n");
			fflush(stderr);
		}
		return -1;
	}
	pthread_mutex_lock(&(file->mutex));
	if ((file->datainfo & O_LOCK) && file->client == 0)
	{
		fprintf(stderr, "server_unlockFile : file %s is already locked\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (file->client != client_socket)
	{
		fprintf(stderr, "server_unlockFile : file %s is open by another client\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	file->datainfo &= ~((unsigned int)0 | O_LOCK);
	pthread_mutex_unlock(&(file->mutex));
	return 0;
}
int server_closeFile(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_closeFile : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_closeFile : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_closeFile : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	const size_t filename_len = (request >> SAPI_MSSLEN_SHFT) + 1;
	char filename[filename_len];
	memset(filename, 0, filename_len);
	if (read(client_socket, &filename, filename_len - 1) == -1)
	{
		perror("server_closeFile : read");
		fprintf(stderr, "server_closeFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	u_file_data *file = fu_getfile(storage, filename);
	if (file == NULL)
	{
		if (server_sendresponse(SAPI_FNF, client_socket) == -1)
		{
			fprintf(stderr, "server_closeFile : server_sendresponse returned an error\n");
			fflush(stderr);
		}
		return -1;
	}
	pthread_mutex_lock(&(file->mutex));
	if ((file->datainfo & O_LOCK) && file->client == 0)
	{
		fprintf(stderr, "server_closeFile : file %s is already locked\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (file->client != client_socket)
	{
		fprintf(stderr, "server_closeFile : file %s is open by another client\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	file->client = 0;
	pthread_mutex_unlock(&(file->mutex));
	return 0;
}
int server_removeFile(int request, int client_socket, u_file_storage* storage)
{
	if (request == 0)
	{
		fprintf(stderr, "server_removeFile : param request == 0\n");
		fflush(stderr);
		return -1;
	}
	if (client_socket < 0)
	{
		fprintf(stderr, "server_removeFile : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (storage == NULL)
	{
		fprintf(stderr, "server_removeFile : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	const size_t filename_len = (request >> SAPI_MSSLEN_SHFT) + 1;
	char filename[filename_len];
	memset(filename, 0, filename_len);
	if (read(client_socket, &filename, filename_len - 1) == -1)
	{
		perror("server_removeFile : read");
		fprintf(stderr, "server_removeFile : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	u_file_data *file = fu_getfile(storage, filename);
	if (file == NULL)
	{
		if (server_sendresponse(SAPI_FNF, client_socket) == -1)
		{
			fprintf(stderr, "server_removeFile : server_sendresponse returned an error\n");
			fflush(stderr);
		}
		return -1;
	}
	pthread_mutex_lock(&(file->mutex));
	if ((file->datainfo & O_LOCK) && file->client == 0)
	{
		fprintf(stderr, "server_removeFile : file %s is already locked\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (file->client != client_socket)
	{
		fprintf(stderr, "server_removeFile : file %s is open by another client\n", filename);
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	if (fu_removefile(storage, filename) == -1)
	{
		fprintf(stderr, "server_removeFile : fu_removefile returned an error\n");
		fflush(stderr);
		pthread_mutex_unlock(&(file->mutex));
		return -1;
	}
	pthread_mutex_unlock(&(file->mutex));
	return 0;
}
int server_evictlist(u_list *savelist, size_t bytes_to_free)
{
	
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

	CHECK_BADVAL_PERROR_EXIT(
		infos.server_socket_fd = create_server_socket(setts), 
		-1, "init_server_infos : create_server_socket"
	);
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
		infos.worker_locks = calloc(infos.nworkers, sizeof(pthread_mutex_t)), 
		NULL, "init_server_infos : calloc"
	);
	CHECK_BADVAL_PERROR_EXIT(
		infos.worker_conds = calloc(infos.nworkers, sizeof(pthread_cond_t)), 
		NULL, "init_server_infos : calloc"
	);
	for (int i = 0; i < infos.nworkers; i++)
	{
		pthread_mutex_init(infos.worker_locks + i, NULL);
		pthread_cond_init(infos.worker_conds + i, NULL);
	}
	infos.server_quit = (infos.server_hu = 0);
	infos.pollarr = pu_initarr(setts->maxClientCount);
	infos.storage = fu_init_file_storage(setts->maxFileCount, setts->avaiableMemory);

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
	fu_storage_free(&(infos->storage));
	pu_free(&(infos->pollarr));

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
LOGFILE_NAME = %s\n\
CLIENT_MAX = %d\n", 
		setts->nworkers, 
		setts->avaiableMemory, 
		setts->maxFileCount, 
		setts->socket_name,
		setts->logfile_name, 
		setts->maxClientCount
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
			server_skipline(configFile);
			break;
		case '\n': // emptyline
			break;
		default:
			CHECK_BADVAL_PERROR_EXIT(
				fseek(configFile, -1, SEEK_CUR),
				-1, "parse_settings : fseek"
			)
			if (server_readword(configFile, &tempRead) != 0)
			{
				if (tempRead != NULL) free(tempRead);
				fclose(configFile);
				CHECK_ERRNO_EXIT(-1, "parse_settings : fclose");
				return (server_settings) {0};
			}
			get_setting(&tempRead, configFile, &settings);
			server_skipline(configFile);
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
	else if (!strcmp(*str, CONFIG_CLIENT_MAX))
	{
		if (fscanf(fstream, "%d", &int_sett) != 1) return;
		setts->maxClientCount = int_sett;
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
int server_sendresponse(s_message response, int client_socket)
{
	if (client_socket < 0)
	{
		fprintf(stderr, "server_sendresponse : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (write(client_socket, &response, sizeof(s_message)) == -1)
	{
		perror("server_sendresponse : write");
		fprintf(stderr, "server_sendresponse : write returned an error\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}
int server_senddata(void *data, size_t datalen, int client_socket)
{
	if (client_socket < 0)
	{
		fprintf(stderr, "server_senddata : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	if (data == NULL)
	{
		fprintf(stderr, "server_sendresponse : param data == NULL\n");
		fflush(stderr);
		return -1;
	}
	if (datalen == 0)
	{
		fprintf(stderr, "server_sendresponse : param datalen == 0\n");
		fflush(stderr);
		return -1;
	}
	if (write(client_socket, &datalen, sizeof(size_t)) == -1)
	{
		perror("server_senddata : write");
		fprintf(stderr, "server_senddata : write returned an error\n");
		fflush(stderr);
		return -1;
	}
	if (write(client_socket, data, datalen) == -1)
	{
		perror("server_senddata : write");
		fprintf(stderr, "server_senddata : write returned an error\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}
int server_getresponse(int client_socket)
{
	if (client_socket < 0)
	{
		fprintf(stderr, "server_getresponse : param client_socket < 0\n");
		fflush(stderr);
		return -1;
	}
	size_t response;
	if (read(client_socket, &response, sizeof(size_t)) == -1)
	{
		perror("server_getresponse : read");
		fprintf(stderr, "server_getresponse : read returned an error\n");
		fflush(stderr);
		return -1;
	}
	return (int) response;
}
int server_skipline(FILE *fstream)
{
    fscanf(fstream, "%*[^\n]");
    fscanf(fstream, "%*c");
    CHECK_ERRNO_RETURN(-1, "f_skipline");

    return 0;
}

int server_readline(FILE *fstream, char **str)
{
    int lineLen = 0;
    
    CHECK_BADVAL_PERROR_RETURN(
        (lineLen = fscanf(fstream, "%*[^\n]")), 
        -1, "fscanf", -1
    );
    CHECK_BADVAL_PERROR_RETURN(
        (0 != fseek(fstream, -lineLen, SEEK_CUR)), 
        1, "fseek", -1
    );
    CHECK_BADVAL_PERROR_EXIT(
        (*str = (char *)malloc((lineLen + 1) * sizeof(char))),
        NULL, "malloc"
    );
    CHECK_COND_RETURN(
        fread((void *)*str, sizeof(char), lineLen, fstream) < lineLen, 
        -1, "fread"
    );
    return 0;
}
    
int server_readword(FILE *fstream, char **str)
{
    int wordLen = 0;
    
    CHECK_BADVAL_PERROR_RETURN(
        fscanf(fstream, "%*[^ ,;=\r\n]%n", &wordLen), 
        -1, "fscanf", -1
    );
    CHECK_BADVAL_PERROR_RETURN(
        (0 != fseek(fstream, -wordLen, SEEK_CUR)), 
        1, "fseek", -1
    );
    CHECK_BADVAL_PERROR_EXIT(
        (*str = (char *)malloc((wordLen + 1) * sizeof(char))),
        NULL, "malloc"
    );
    CHECK_COND_RETURN(
        fread((void *)*str, sizeof(char), wordLen, fstream) < wordLen, 
        -1, "fread"
    );
    (*str)[wordLen] = (char) 0;

    return 0;
}