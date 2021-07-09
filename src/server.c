#include <server.h>
#include <unistd.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/un.h>
#include <string.h>
#include <file_utils.h>
#include <error_utils.h>
#include <thread_utils.h>
#include <mem_utils.h>
#include <poll_utils.h>
#include <server_message.h>

int main(int argc, char** argv)
{
	DEBUG(puts("main"));

	static server_settings settings;
	static server_infos infos;

	CHECK_PERROR_EXIT(init_server_settings(&settings),
		"init_server_settings at main");
	CHECK_PERROR_EXIT(init_server_infos(&settings, &infos),
		"init_server_infos at main");

	CHECK_PERROR_EXIT((tu_create_thread_detached(&server_signal_handler, (void*)&infos)),
		"tu_create_thread_detached at main");
	CHECK_PERROR_EXIT((ignore_signals()),
		"ignore_signals at main");

	server_dispatcher(&infos);

	join_workers(&infos);

	CHECK_PERROR_EXIT(close(infos.server_socket_fd),
		"close at main");
	CHECK_PERROR_EXIT(unlink(settings.socket_name),
		"unlink at main");
	CHECK_PERROR_EXIT(free_server_infos(&infos),
		"free_server_infos at main");

	return EXIT_SUCCESS;
}

int init_server_settings(server_settings* setts)
{
	DEBUG(puts("init_server_settings"));

	FILE* configFile = NULL;
	CHECK_PERROR_EXIT((configFile = fopen(SERVER_CONFIGFILE_PATH, "r")) == NULL,
		"fopen at init_server_settings");

	//size_t fstream_pos = 0;
	char* tempRead = NULL;

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
			CHECK_PERROR_EXIT(fseek(configFile, -1, SEEK_CUR) == -1,
				"fseek at init_server_settings");
			if (server_readword(configFile, &tempRead) != 0)
			{
				if (tempRead != NULL)
				{ free(tempRead); }
				fclose(configFile);
				CHECK_ERRNO_PERROR_EXIT("fclose at init_server_settings");
				return -1;
			}
			get_setting(&tempRead, configFile, setts);
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
	CHECK_ERRNO_PERROR_EXIT("fclose at init_server_settings");
	return 0;
}

int init_server_infos(server_settings* setts, server_infos* infos)
{
	DEBUG(puts("init_server_infos"));

	CHECK_PERROR_EXIT(
		(infos->server_socket_fd = create_server_socket(setts)) == -1,
		"create_server_socket at init_server_infos"
	);
	infos->n_workers = setts->n_workers;

	infos->workers = mu_calloc(infos->n_workers * sizeof(pthread_t));
	infos->workers_clients = mu_calloc(infos->n_workers * sizeof(int));
	infos->worker_locks = mu_calloc(infos->n_workers * sizeof(pthread_mutex_t));
	infos->worker_conds = mu_calloc(infos->n_workers * sizeof(pthread_cond_t));

	for (int i = 0; i < infos->n_workers; i++)
	{
		pthread_mutex_init(infos->worker_locks + i, NULL);
		pthread_cond_init(infos->worker_conds + i, NULL);
	}

	infos->server_quit = (infos->server_hu = 0);
	infos->poll_arr = pu_initarr(setts->maxClientCount);
	infos->storage = fu_init_file_storage(setts->maxFileCount, setts->available_Memory);

	return 0;
}

void server_dispatcher(server_infos* infos)
{
	DEBUG(puts("Dispatcher"));

	spawn_workers(infos);

	pu_add(&(infos->poll_arr), infos->server_socket_fd, POLL_IN);

	int poll_ready = 0;

	while (!(infos->server_hu && pu_isempty(&(infos->poll_arr))) && !infos->server_quit)
	{
		if (infos->server_hu)
		{ pu_remove(&(infos->poll_arr), infos->server_socket_fd); }

		for (size_t i = 0; i < infos->poll_arr.len; i++) printf("%d->", infos->poll_arr.arr[i].fd);
		printf("\n");

		poll_ready = poll(infos->poll_arr.arr, infos->poll_arr.len, 1000);

		for (size_t i = 0; poll_ready && i < infos->poll_arr.len; i++)
		{
			if (infos->poll_arr.arr[i].revents & POLLIN)
			{
				if (infos->poll_arr.arr[i].fd != infos->server_socket_fd)
				{
					if (assign_client(infos, infos->poll_arr.arr[i].fd))
					{
						fprintf(stderr, "assign_client at server_dispatcher");
						fflush(stderr);
					}
					else
					{
						pu_remove(&(infos->poll_arr), infos->poll_arr.arr[i].fd);
					}
				}
				else
				{
					if (!pu_isfull(&(infos->poll_arr)))
					{
						pu_add(&(infos->poll_arr),
							accept(infos->server_socket_fd, NULL, 0),
							POLL_IN | POLL_HUP
						);
					}
				}
				infos->poll_arr.arr[i].revents &= 0;
			}
			else if (infos->poll_arr.arr[i].revents & POLLHUP)
			{
				close(infos->poll_arr.arr[i].fd);
				pu_remove(&(infos->poll_arr), infos->poll_arr.arr[i].fd);
			}
		}
	}
	if (infos->server_hu)
	{ infos->server_quit = 1; }
}

void* server_worker(void* worker_arg)
{
	DEBUG(puts("Worker"));
	ignore_signals();

	struct worker_arg* worker_infos = (struct worker_arg*)worker_arg;
	s_message request;
	int serve_ret;
	struct timespec cond_time;
	cond_time.tv_sec = 0;
	cond_time.tv_nsec = 0xFFFF;

	while (!worker_infos->infos->server_quit)
	{
		pthread_mutex_lock(worker_infos->infos->worker_locks + worker_infos->worker_id);

		while ((worker_infos->infos->workers_clients)[worker_infos->worker_id] == 0)
		{
			if (worker_infos->infos->server_quit)
			{
				mu_free(worker_infos);
				pthread_exit(NULL);
			}
			pthread_cond_timedwait(
				worker_infos->infos->worker_conds + worker_infos->worker_id,
				worker_infos->infos->worker_locks + worker_infos->worker_id,
				&cond_time
			);
		}

		DEBUG(
			fprintf(stdout, "worker #%d assigned to serve %d\n",
				worker_infos->worker_id, worker_infos->infos->workers_clients[worker_infos->worker_id]);
			fflush(stdout);
		)

		request = 0;
		if ((read((worker_infos->infos->workers_clients)[worker_infos->worker_id],
			&request, sizeof(s_message)) < sizeof(s_message)))
		{
			if (errno)
			{
				perror("read at server_worker");
				errno = 0;
			}
			continue;
		}

		DEBUG(
			fprintf(stdout, "worker #%d, received request %X from %d\n",
				worker_infos->worker_id, request,
				(worker_infos->infos->workers_clients)[worker_infos->worker_id]);
			fflush(stdout);
		)

		if (request)
		{

			serve_ret = serve(request,
				(worker_infos->infos->workers_clients)[worker_infos->worker_id],
				&(worker_infos->infos->storage)
			);
			fprintf(stdout, "worker #%d, service %X from %d returned %d\n",
				worker_infos->worker_id, request,
				(worker_infos->infos->workers_clients)[worker_infos->worker_id],
				serve_ret);
			fflush(stdout);
			pu_add(&(worker_infos->infos->poll_arr),
				(worker_infos->infos->workers_clients)[worker_infos->worker_id],
				POLLIN | POLLHUP);
		}

		(worker_infos->infos->workers_clients)[worker_infos->worker_id] = 0;
		pthread_mutex_unlock(worker_infos->infos->worker_locks + worker_infos->worker_id);
	}

	free(worker_infos);

	pthread_exit(NULL);
}

void* server_signal_handler(void* infos)
{
	DEBUG(puts("Signal_handler"));

	server_infos* s_infos = (server_infos*)infos;

	int sig = 0;
	sigset_t set;
	sigemptyset(&set);

	if (sigaddset(&set, SIGINT) || sigaddset(&set, SIGQUIT) || sigaddset(&set, SIGHUP))
	{
		perror("server_signal_handler : sigaddset");
	}

	if (sigwait(&set, &sig))
	{
		perror("server_signal_handler : sigwait");
	}

	DEBUG(printf("\nserver_signal_handler received signal %d\n", sig));

	if (sig == SIGINT || sig == SIGQUIT)
	{
		s_infos->server_quit = 1;
	}
	else
	{
		s_infos->server_hu = 1;
	}

	pthread_exit(NULL);
}

int serve(s_message request, int client_socket, u_file_storage* storage)
{
	DEBUG(puts("serve"));

	if (storage == NULL)
	{
		fprintf(stderr, "serve : param storage == NULL\n");
		fflush(stderr);
		return -1;
	}
	unsigned char op = request & SAPI_OPMASK;
	switch (op)
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
	return 0;
}

int spawn_workers(server_infos* infos)
{
	DEBUG(puts("spawn_workers"));

	struct worker_arg* arg;
	for (int i = 0; i < infos->n_workers; i++)
	{
		arg = (struct worker_arg*)malloc(sizeof(struct worker_arg));
		arg->worker_id = i;
		arg->infos = infos;
		(infos->workers)[i] = tu_create_thread(&server_worker, (void*)(arg));
	}
	return 0;
}

int join_workers(server_infos* infos)
{
	DEBUG(puts("join_workers"));

	int ret;

	for (int i = 0; i < infos->n_workers; i++)
	{
		if ((ret = pthread_join((infos->workers)[i], NULL)))
		{
			switch (ret)
			{
			case EDEADLK:
				puts(" A  deadlock  was  detected (e.g., two threads tried to join with "
					 "each other); or thread specifies the calling thread.");
				break;
			case EINVAL:
				puts("thread is not a joinable thread.");
				break;
			case ESRCH:
				puts("No thread with the ID thread could be found");
				break;
			default :
				break;
			}
			perror("pthread_join at join_workers");
			errno = 0;
		}
	}
	return 0;
}

int assign_client(server_infos* infos, int client)
{
	DEBUG(puts("assign_client"));

	if (infos == NULL || client < 0)
	{ return -1; }

	for (int i = 0;; i = ((i + 1) % infos->n_workers))
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
	DEBUG(puts("ignore_signals"));

	sigset_t set;

	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGHUP);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGPIPE);

	return pthread_sigmask(SIG_BLOCK, &set, NULL);
}

int free_server_infos(server_infos* infos)
{
	DEBUG(puts("free_server_infos"));

	if (infos == NULL)
	{
		return -1;
	}

	if (infos->workers != NULL)
	{ free(infos->workers); }
	if (infos->workers_clients != NULL)
	{ free(infos->workers_clients); }
	for (int i = 0; i < infos->n_workers; i++)
	{
		pthread_cond_destroy(infos->worker_conds + i);
		pthread_mutex_destroy(infos->worker_locks + i);
	}
	if (infos->worker_conds != NULL)
	{ free(infos->worker_conds); }
	if (infos->worker_locks != NULL)
	{ free(infos->worker_locks); }
	fu_storage_free(&(infos->storage));
	pu_free(&(infos->poll_arr));

	return 0;
}

void print_server_settings(server_settings* setts)
{
	fprintf(
		stdout,
		"NWORKERS = %d\n\
CAPACITY = %dMB\n\
FILECAPACITY = %d files\n\
SOCKET_NAME = %s\n\
LOGFILE_NAME = %s\n\
CLIENT_MAX = %d\n",
		setts->n_workers,
		setts->available_Memory,
		setts->maxFileCount,
		setts->socket_name,
		setts->logfile_name,
		setts->maxClientCount
	);
	fflush(stdout);
}

int write_log(const char* op, server_settings* setts)
{
	FILE* fOut;
	CHECK_PERROR_RETURN(
		(fOut = fopen(setts->logfile_name, "a")) == NULL,
		"write_log : fopen", -1
	);

	time_t currTime;
	time(&currTime);
	char* currTimeString = ctime(&currTime); //The return value points to a statically allocated string
	if (errno)
	{
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

void get_setting(char** str, FILE* fstream, server_settings* setts)
{
	DEBUG(puts("get_settings"));

	int int_sett = 0;

	fscanf(fstream, "%*[ ,;=\n\r]");
	if (feof(fstream))
	{ return; }

	if (!strcmp(*str, CONFIG_NWORKERS))
	{
		if (fscanf(fstream, "%d", &int_sett) != 1)
		{ return; }
		setts->n_workers = int_sett;
	}
	else if (!strcmp(*str, CONFIG_CAPACITY))
	{
		if (fscanf(fstream, "%d", &int_sett) != 1)
		{ return; }
		setts->available_Memory = int_sett;
	}
	else if (!strcmp(*str, CONFIG_CLIENT_MAX))
	{
		if (fscanf(fstream, "%d", &int_sett) != 1)
		{ return; }
		setts->maxClientCount = int_sett;
	}
	else if (!strcmp(*str, CONFIG_FILECAPACITY))
	{
		if (fscanf(fstream, "%d", &int_sett) != 1)
		{ return; }
		setts->maxFileCount = int_sett;
	}
	else if (!strcmp(*str, CONFIG_SOCKET_NAME))
	{
		fscanf(fstream, "%*[^ ,;=\n\r]%n", &int_sett);
		fseek(fstream, -int_sett, SEEK_CUR);
		CHECK_ERRNO_PERROR_EXIT("fseek : get_settings");

		if (feof(fstream))
		{ return; }
		CHECK_PERROR_EXIT(
			fread(&(setts->socket_name), sizeof(char), int_sett < 64 ? int_sett : 63,
				fstream) < int_sett, "fread : get_settings"
		);

	}
	else if (!strcmp(*str, CONFIG_LOGFILE_NAME))
	{
		fscanf(fstream, "%*[^ ,;=\n\r]%n", &int_sett);
		fseek(fstream, -int_sett, SEEK_CUR);
		CHECK_ERRNO_PERROR_EXIT("fseek : get_settings");

		if (feof(fstream))
		{ return; }
		CHECK_PERROR_EXIT(
			fread(&(setts->logfile_name), sizeof(char), int_sett < 64 ? int_sett : 63,
				fstream) < int_sett, "fread : get_settings"
		);

	}
	else
	{
		puts("SettingFormat not recongnized");
	}
}

int create_server_socket(server_settings* setts)
{
	DEBUG(puts("create_server_socket"));
	DEBUG(unlink(setts->socket_name));

	int socket_res;

	struct sockaddr_un sa;
	memset(&sa, 0, sizeof(struct sockaddr_un));

	strncpy(sa.sun_path, setts->socket_name, sizeof(sa.sun_path) - 1);

	sa.sun_family = AF_UNIX;
	CHECK_PERROR_RETURN(
		(socket_res = socket(AF_UNIX, SOCK_STREAM, 0)) == -1,
		"create_server_socket : socket", -1
	);

	if (bind(socket_res, (struct sockaddr*)&sa, sizeof(sa)))
	{
		perror("create_server_socket : bind");
		unlink(sa.sun_path);
		return -1;
	}

	if (listen(socket_res, 256))
	{
		perror("create_server_socket : listen");
		unlink(sa.sun_path);
		return -1;
	}

	return socket_res;
}

void server_skipline(FILE* fstream)
{
	fscanf(fstream, "%*[^\n]");
	fscanf(fstream, "%*c");
}

int server_readline(FILE* fstream, char** str)
{
	int lineLen = 0;
	CHECK_PERROR_RETURN(
		(lineLen = fscanf(fstream, "%*[^\n]")) == -1,
		"fscanf at server_readline", -1
	);
	CHECK_PERROR_RETURN(
		(0 != fseek(fstream, -lineLen, SEEK_CUR)),
		"fseek at server_readline", -1
	);

	*str = (char*)mu_malloc((lineLen + 1) * sizeof(char));

	CHECK_PERROR_EXIT(
		fread((void*)*str, sizeof(char), lineLen, fstream) < lineLen,
		"fread at server_readline"
	);
	(*str)[lineLen] = (char)0;

	return 0;
}

int server_readword(FILE* fstream, char** str)
{
	int wordLen = 0;

	CHECK_PERROR_RETURN(
		fscanf(fstream, "%*[^ ,;=\r\n]%n", &wordLen) == -1,
		"fscanf at server_readword", -1);
	CHECK_PERROR_RETURN(
		(0 != fseek(fstream, -wordLen, SEEK_CUR)),
		"fseek at server_readword", -1);
	*str = (char*)mu_malloc((wordLen + 1) * sizeof(char));
	CHECK_PERROR_RETURN(
		fread((void*)*str, sizeof(char), wordLen, fstream) < wordLen,
		"fread at server_readword", -1
	);
	(*str)[wordLen] = (char)0;

	return 0;
}

int server_openFile(s_message message, int client, u_file_storage* storage)
{
	DEBUG(puts(__func__));
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_openFile : wrong params\n");
		fflush(stderr);
		return -1;
	}
	char* path_to_open = sapi_getpath(message, client);
	if (path_to_open == NULL)
	{
		if (sapi_respond(SAPI_FAILURE, client))
		{
			fprintf(stderr, "sapi_respond at server_openFile\n");
			fflush(stderr);
			return SAPI_FAILURE;
		}
		return SAPI_FAILURE;
	}

	u_file_data* file = fu_getfile(storage, path_to_open);

	if (file == NULL && !(message & O_CREATE))
	{
		fprintf(stderr, "server_openFile : file %s not found\n", path_to_open);
		fflush(stderr);
		mu_free(path_to_open);
		if (sapi_respond(SAPI_FNF, client))
		{
			fprintf(stderr, "sapi_respond at server_openFile\n");
			fflush(stderr);
			return SAPI_FAILURE;
		}
		return SAPI_FNF;
	}

	if (file != NULL)
	{
		mu_free(path_to_open);

		mutex_lock(file->mutex);

		if (file->client != -1)
		{
			fprintf(stderr, "server_openFile : file is already open by %d\n", file->client);
			fflush(stderr);
			mu_free(path_to_open);
			mutex_unlock(file->mutex);
			if (sapi_respond(SAPI_ALREADY, client))
			{
				fprintf(stderr, "sapi_respond at server_openFile\n");
				fflush(stderr);
				return SAPI_FAILURE;
			}
			return SAPI_ALREADY;
		}
		if (file->data_info & O_LOCK)
		{
			fprintf(stderr, "server_openFile : file is already locked\n");
			fflush(stderr);
			mu_free(path_to_open);
			mutex_unlock(file->mutex);
			if (sapi_respond(SAPI_LOCKED, client))
			{
				fprintf(stderr, "sapi_respond at server_openFile\n");
				fflush(stderr);
				return SAPI_FAILURE;
			}
			return SAPI_LOCKED;
		}

		file->client = client;
		if (message & O_LOCK)
		{
			file->data_info &= O_LOCK;
		}

		mutex_unlock(file->mutex);
	}
	else
	{
		u_file_data* new_file = fu_alloc_file_data(NULL, 0);
		new_file->client = client;
		if (message & O_LOCK)
		{
			new_file->data_info &= O_LOCK;
		}
		fu_add_file(storage, new_file, path_to_open);
	}

	if (sapi_respond(SAPI_SUCCESS, client))
	{
		fprintf(stderr, "sapi_respond at server_openFile\n");
		fflush(stderr);
		return SAPI_FAILURE;
	}

	return 0;
}

int server_closeFile(s_message message, int client, u_file_storage* storage)
{
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_closeFile : wrong params\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}

int server_readFile(s_message message, int client, u_file_storage* storage)
{
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_readFile : wrong params\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}

int server_readNFiles(s_message message, int client, u_file_storage* storage)
{
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_readNFiles : wrong params\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}

int server_writeFile(s_message message, int client, u_file_storage* storage)
{
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_writeFile : wrong params\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}

int server_appendToFile(s_message message, int client, u_file_storage* storage)
{
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_appendToFile : wrong params\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}

int server_lockFile(s_message message, int client, u_file_storage* storage)
{
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_lockFile : wrong params\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}

int server_unlockFile(s_message message, int client, u_file_storage* storage)
{
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_unlockFile : wrong params\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}

int server_removeFile(s_message message, int client, u_file_storage* storage)
{
	if (message == 0 || client == 0 || storage == NULL)
	{
		fprintf(stderr, "server_removeFile : wrong params\n");
		fflush(stderr);
		return -1;
	}
	return 0;
}

char* sapi_getpath(s_message message, int client)
{
	if (message == 0 || client == 0)
	{
		fprintf(stderr, "sapi_getpath : wrong params\n");
		fflush(stderr);
		return NULL;
	}
	size_t path_len = message >> SAPI_MSSLEN_SHFT;
	char* path = mu_calloc(path_len + 1);
	int read_len = 0;
	if ((read_len = (int)read(client, path, path_len)) < path_len)
	{
		if (read_len < 0)
		{ perror("read at sapi_getpath"); }
		fprintf(stderr, "read at sapi_getpath returned %d, instead of %zu\n", read_len, path_len);
		fflush(stderr);
		mu_free(path);
		return NULL;
	}
	return path;
}

int sapi_respond(s_message message, int client)
{
	if (client == 0)
	{
		fprintf(stderr, "sapi_respond : wrong params\n");
		fflush(stderr);
		return -1;
	}

	int write_len = 0;

	if ((write_len = (int)write(client, &message, sizeof(s_message))) < sizeof(message))
	{
		if (write_len < 0)
		{ perror("read at sapi_respond"); }
		fprintf(stderr, "write at sapi_respond returned %d, instead of %zu\n", write_len, sizeof(s_message));
		fflush(stderr);
		return -1;
	}

	return 0;
}
