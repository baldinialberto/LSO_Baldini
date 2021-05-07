#include <server_utils.h>

int main(int argc, char** argv)
{
	server_settings settings = parse_settings();
	printf(
		"NWORKERS = %d\n\
CAPACITY = %dMB\n\
SOCKET_NAME = %s\n", 
		settings.nworkers, 
		settings.MB_dim, 
		settings.socket_name
	);
	return EXIT_SUCCESS;
}

