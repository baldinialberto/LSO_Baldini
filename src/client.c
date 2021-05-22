#include <client.h>

int main(int argc, char** argv)
{
	assert(sizeof(server_command_t) == 4);

	if (openConnection("MyServerSocket.socket", 200, (struct timespec){0xFFFF, 0}))
	{
		printf("Unable to connect to server");
		return 1;
	}

	openFile("Pippo.txt", O_CREATE_FLAG | O_LOCK_FLAG);
	unlockFile("Pippo.txt");
	closeFile("Pippo.txt");

	closeConnection("MyServerSocket.socket");

	return 0;
}