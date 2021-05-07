#include "../include/server_utils.h"


server_settings parse_settings()
{
	server_settings settings;

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