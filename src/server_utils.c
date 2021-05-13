#include <server_utils.h>

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
		NULL, "main : calloc"
	);
	CHECK_BADVAL_PERROR_EXIT(
		infos.workers_clients = calloc(infos.nworkers, sizeof(int)), 
		NULL, "main : calloc"
	);
	infos.server_quit = (infos.server_hu = 0);

	return infos;
}

int free_server_infos(server_infos *infos)
{
	if (infos->workers == NULL && infos->workers_clients == NULL)
		return -1;
	if (infos->workers != NULL) free(infos->workers);
	if (infos->workers_clients != NULL) free(infos->workers_clients);
	return 0;
}

void print_server_settings(server_settings *setts)
{
	fprintf(
		stdout,
		"NWORKERS = %d\n\
CAPACITY = %dMB\n\
SOCKET_NAME = %s\n\
LOGFILE_NAME = %s\n", 
		setts->nworkers, 
		setts->MB_dim, 
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
		setts->MB_dim = int_sett;
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