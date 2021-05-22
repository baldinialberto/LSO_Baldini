#include <client.h>

//static int socket_fd;

int main(int argc, char** argv)
{
	socket_fd = 0;
	assert(sizeof(server_command_t) == 4);
	struct timespec connectionTime;
	connectionTime.tv_sec = 0;
	connectionTime.tv_nsec = 999999999;

	if (openConnection("MyServerSocket.socket", 200, connectionTime))
	{
		printf("Unable to connect to server\n");
		return 1;
	}
	else 
		printf("Connected\n");


	printf("Connected to %d\n", socket_fd);
	//openFile("Pippo.txt", O_CREATE_FLAG | O_LOCK_FLAG);
	//unlockFile("Pippo.txt");
	//closeFile("Pippo.txt");

	sleep(5);

	if (closeConnection("MyServerSocket.socket"))
		printf("Unable to disconnect\n");
	else
		printf("disconnected\n");

	return 0;
}