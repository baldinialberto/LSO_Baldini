#include <serverapi.h>


int socket_fd;

int openConnection(const char* sockname, int msec, const struct timespec abstime)
{
    if (sockname == NULL)
    {
        fprintf(stderr, "openConnection : param sockname == NULL\n");
        fflush(stderr);
        return -1;
    }
    
    struct sockaddr_un sa;
	strncpy(sa.sun_path, sockname, sizeof(sa.sun_path)-1);
	sa.sun_family = AF_UNIX;

    if (socket_fd = socket(AF_UNIX, SOCK_STREAM, 0) == -1)
    {
        fprintf(stderr, "openConnection : socket returned -1\n");
        fflush(stderr);
        perror("openConnection : socket");
        return -1;
    }

    struct timespec inter_requests_time, remaining_time;
    inter_requests_time.tv_nsec = msec * 1000;
    if (inter_requests_time.tv_nsec > 999999999 || inter_requests_time.tv_nsec <= 0)
        inter_requests_time.tv_nsec = 1000000;
    remaining_time.tv_nsec = abstime.tv_nsec;

    while (connect(socket_fd, (struct sockaddr *)&sa, sizeof(sa)) == -1 && 
    remaining_time.tv_nsec > 0)
	{
		if (errno == ENOENT || errno == ECONNREFUSED) 
        {
            fprintf(stdout, "Not connected\n");
			nanosleep(&inter_requests_time, NULL);
            remaining_time.tv_nsec -= inter_requests_time.tv_nsec;
        }
		else
        {
            perror("openConnection : connect");
			return(-1);
        }
	}

    if (remaining_time.tv_nsec > 0)
        return 0;

    return -1;;
}
int closeConnection(const char* sockname)
{
    if (sockname == NULL)
    {
        fprintf(stderr, "closeConnection : param sockname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "closeConnection : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return close(socket_fd);
}
int openFile(const char* pathname, int flags)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "openFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "openFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }

   

    return 0;
}
int readFile(const char* pathname, void** buf, size_t* size)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "readFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (buf == NULL)
    {
        fprintf(stderr, "readFile : param buf == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (size == NULL)
    {
        fprintf(stderr, "readFile : param size == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "readFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }

    return 0;
}
int readNFiles(int N, const char *dirname)
{
    if (N == 0)
    {
        fprintf(stderr, "readNFiles : param N == 0 -> nothing to do\n");
        fflush(stderr);
        return 0;
    }
    if (dirname == NULL)
    {
        fprintf(stderr, "readNFiles : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "readNFiles : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return 0;
}
int writeFile(const char* pathname, const char* dirname)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "writeFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (dirname == NULL)
    {
        fprintf(stderr, "writeFile : param dirname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "readNFiles : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return 0;
}
int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "appendToFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (dirname == NULL)
    {
        fprintf(stderr, "appendToFile : param dirname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (buf == NULL && size != 0)
    {
        fprintf(stderr, "appendToFile : param buf == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (size == 0 && buf != NULL)
    {
        fprintf(stderr, "appendToFile : param size == 0\n");
        fflush(stderr);
        return -1;
    }
    if (size == 0 && buf == NULL)
    {
        fprintf(stderr, "appendToFile : param buf == NULL && size == 0 -> nothing to do\n");
        fflush(stderr);
        return 0;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "appendToFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return 0;
}
int lockFile(const char* pathname)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "lockFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "lockFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return 0;
}
int unlockFile(const char* pathname)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "unlockFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "unlockFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return 0;
}
int closeFile(const char* pathname)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "closeFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "closeFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return 0;
}
int removeFile(const char* pathname)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "closeFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!socket_fd)
    {
        fprintf(stderr, "closeFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return 0;
}
