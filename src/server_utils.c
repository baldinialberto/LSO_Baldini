#include <server_utils.h>

void print_server_settings(server_settings *setts)
{
	fprintf(
		stdout,
		"NWORKERS = %d\n\
CAPACITY = %dMB\n\
SOCKET_NAME = %s\n", 
		setts->nworkers, 
		setts->MB_dim, 
		setts->socket_name
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
			//fstream_pos = ftell(configFile);
			CHECK_ERRNO_CALL(
				fclose(configFile),
				"ftell"
			);
			CHECK_BADVAL_PERROR(
				fseek(configFile, -1, SEEK_CUR),
				-1, "fseek"
			)
			if (f_readword(configFile, &tempRead) != 0)
			{
				if (tempRead != NULL) free(tempRead);
				fclose(configFile);
				CHECK_ERRNO_EXIT(-1, "fclose");
				return (server_settings) {0};
			}
			get_setting(&tempRead, configFile, &settings);
			f_skipline(configFile);
			break;
		}
	}
	fclose(configFile);
	CHECK_ERRNO_RETURN((server_settings) {0}, "fclose");
	return settings;
}
int write_settings(server_settings* settings)
{
	return 0;
}

int write_log(const char* op, server_settings *settings)
{
	FILE* fOut = fopen(SERVER_CONFIGFILE_PATH, "a");
	if (fOut == NULL) {
		perror("write_log");
		errno = 0;
		return -1;
	}
	time_t currTime;
	time(&currTime);
	const char* currTimeString = ctime(&currTime);
	// use String module
	return 0;
}

void get_setting(char** str, FILE *fstream, server_settings *settings)
{
	int int_sett = 0;

	fscanf(fstream, "%*[ \n\r]");
	if (feof(fstream)) return;

	if (!strcmp(*str, CONFIG_NWORKERS)) 
	{
		if (fscanf(fstream, "%d", &int_sett) != 1) return;
		settings->nworkers = int_sett;
	} 
	else if (!strcmp(*str, CONFIG_CAPACITY))
	{
		if (fscanf(fstream, "%d", &int_sett) != 1) return;
		settings->MB_dim = int_sett;
	} 
	else if (!strcmp(*str, CONFIG_SOCKET_NAME))
	{
		if (fscanf(fstream, "%*[^ \n\r,;]%n", &int_sett) != 1) return;

		fseek(fstream, -int_sett, SEEK_CUR);
		CHECK_ERRNO_EXIT(-1, "fseek : get_settings");

		if (feof(fstream)) return;
		CHECK_BADVAL_PERROR_EXIT(
			fread(&(settings->socket_name),
			sizeof(char), 
			int_sett < 64 ? int_sett : 63, 
			fstream), 0, 
			"fread : get_settings"
		);
		
	} else {
		puts("SettingFormat not recongnized");
	}
}
