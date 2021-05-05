#include "../include/common.h"
#include "../include/commonio.h"
#include "../include/procs.h"
#include "../include/strings.h"
#include "../include/locks.h"

int main(int argc, char** argv)
{
	pid_t child = fork();
	pthread_mutex_t mutex;

	if (!child)
	{
		string s = create_string_from_literal("I'm Child");
		printf("%d, %s\n", getpid(), s.data);
	}
	else
		printf("%d, I'm Father of %d\n", getpid(), child);

	return EXIT_SUCCESS;
}