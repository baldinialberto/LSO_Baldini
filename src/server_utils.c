#include <server_utils.h>


server_settings parse_settings()
{
	server_settings settings = {0};

	FILE* configFile = NULL;
	CHECK_BADVAL_PERROR_RETURN(
		(configFile = fopen(SERVER_CONFIGFILE_PATH, "r")), 
		NULL, "fopen", settings
	);

	while (!feof(configFile))
	{
		switch (fgetc(configFile))
		{
		case '#': // comment
			f_skipline(configFile);
			break;
		case '\n': // emptyline
			break;
		default:
			CHECK_BADVAL_PERROR_RETURN(
				fseek(configFile, -1, SEEK_CUR),
				-1, "fseek", settings
			);

			break;
		}
	}
	

	return settings;
}
int write_settings(server_settings* settings)
{
	return 0;
}

int write_log(const char* op, server_settings *settings)
{
	FILE* fOut = fopen(settings->logfile_path, "a");
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