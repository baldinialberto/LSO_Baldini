#include "../include/serverapi.h"


int server_socket_fd;

int sapi_sendop(const char *pathname, unsigned char op, unsigned char flags)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "sapi_sendop : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!server_socket_fd)
    {
        fprintf(stderr, "sapi_sendop : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    s_message message = SAPI_MESSAGE_LEN(flags, op, strlen(pathname));
    if (write(server_socket_fd, &message, sizeof(s_message)) == -1)
    {
        fprintf(stderr, "sapi_sendop : write returned an error\n");
        fflush(stderr);
        perror("sapi_sendop : write");
        return -1;
    }
    if (write(server_socket_fd, pathname, strlen(pathname) + 1) == -1)
    {
        fprintf(stderr, "sapi_sendop : write returned an error\n");
        fflush(stderr);
        perror("sapi_sendop : write");
        return -1;
    }
    return 0;
}
int sapi_senddata(void *data, size_t datalen)
{
    if (data == NULL)
    {
        fprintf(stderr, "sapi_senddata : param data == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (datalen == 0)
    {
        fprintf(stderr, "sapi_senddata : param datalen == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!server_socket_fd)
    {
        fprintf(stderr, "sapi_senddata : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    if (write(server_socket_fd, data, datalen) == -1)
    {
        fprintf(stderr, "sapi_senddata : write returned an error\n");
        fflush(stderr);
        perror("sapi_senddata : write");
        return -1;
    }
}
int sapi_getresponse()
{
    s_message message;
    if (read(server_socket_fd, &message, sizeof(s_message)) == -1)
    {
        fprintf(stderr, "sapi_getresponse : read returned an error\n");
        fflush(stderr);
        perror("sapi_getresponse : read");
        return -1;
    }
    return message == SAPI_SUCCESS ? 0 : -1;
}
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

    if (server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0) == -1)
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

    while (connect(server_socket_fd, (struct sockaddr *)&sa, sizeof(sa)) == -1 && 
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
    if (!server_socket_fd)
    {
        fprintf(stderr, "closeConnection : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return close(server_socket_fd);
}
int openFile(const char* pathname, int flags)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "openFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!server_socket_fd)
    {
        fprintf(stderr, "openFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_sendop(pathname, (unsigned char)flags, SAPI_OPENFILE) == -1)
    {
        fprintf(stderr, "openFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_getresponse() == -1)
    {
        fprintf(stderr, "openFile : sapi_getresponse returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddata(pathname, strlen(pathname)) == -1)
    {
        fprintf(stderr, "openFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_getresponse() == -1)
    {
        fprintf(stderr, "openFile : sapi_getresponse returned an error\n");
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
    if (!server_socket_fd)
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
    if (!server_socket_fd)
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
    if (!server_socket_fd)
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
    if (!server_socket_fd)
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
    if (!server_socket_fd)
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
    if (!server_socket_fd)
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
    if (!server_socket_fd)
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
    if (!server_socket_fd)
    {
        fprintf(stderr, "closeFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    return 0;
}
