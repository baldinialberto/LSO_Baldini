#include <serverapi.h>
#define DEBUG(X) X

int socket_fd;

int openConnection(const char* sockname, int msec, const struct timespec abstime)
{
    DEBUG(puts("OpenConnection"));
    struct sockaddr_un sa;
	strncpy(sa.sun_path, sockname, sizeof(sa.sun_path)-1);
	sa.sun_family = AF_UNIX;

    CHECK_BADVAL_PERROR_RETURN(
        socket_fd = socket(AF_UNIX, SOCK_STREAM, 0), 
        -1, "openConnection : socket", -1
    )

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
            printf("Not connected");
			nanosleep(&inter_requests_time, NULL);
            remaining_time.tv_nsec -= inter_requests_time.tv_nsec;
        }
		else
        {
            printf("errno = %d\n", errno);
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
    DEBUG(puts("CloseConnection"));
    if (!socket_fd) return -1;
    return close(socket_fd);
}
int openFile(const char* pathname, int flags)
{
    if (!socket_fd) return NOTCONN_FLAG;

    server_command_t command = OPEN_OP;
    command |= O_LOCK_FLAG;

    unsigned int pathname_len = strlen(pathname);
    if (pathname_len > MESSAGE_MAX_LEN) return -1;
    pathname_len <<= MESSG_SHIFT;

    command |= pathname_len;

    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)pathname, pathname_len * sizeof(char)), 
        -1, -1
    )

    server_command_t res;
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    )

    return res;
}
int readFile(const char* pathname, void** buf, size_t* size)
{
    if (!socket_fd) return NOTCONN_FLAG;

    server_command_t command = READ_OP;
    command |= O_LOCK_FLAG;

    unsigned int pathname_len = strlen(pathname);
    if (pathname_len > MESSAGE_MAX_LEN) return -1;
    pathname_len <<= MESSG_SHIFT;

    command |= pathname_len;

    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)pathname, pathname_len * sizeof(char)), 
        -1, -1
    )

    server_command_t res;
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    )
    if (res & OP_MASK) return res;

    res >>= MESSG_SHIFT; // len of chars readen

    *buf = calloc(res, sizeof(char*));
    *size = (size_t)res;

    CHECK_BADVAL_RETURN(
        read(socket_fd, *buf, res), 
        -1, -1
    )

    return res;
}

int readNFiles(int N, const char *dirname)
{
    return 0;
}

int writeFile(const char* pathname, const char* dirname)
{
    if (!socket_fd) return NOTCONN_FLAG;

    server_command_t command = WRITE_OP;
    command |= O_CREATE_FLAG;
    command |= O_LOCK_FLAG;
    if (dirname != NULL) command |= O_DIR_FLAG;

    unsigned int pathname_len = strlen(pathname);
    if (pathname_len > MESSAGE_MAX_LEN) return -1;
    
    pathname_len <<= MESSG_SHIFT;
    command |= pathname_len;

    // write command
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    // write pathname
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)pathname, pathname_len * sizeof(char)), 
        -1, -1
    )
    
    FILE *file = fopen(pathname, "r");
    if (file == NULL) return -1;

    size_t bytesreadend = 0, datalen = 1025, i = 0;
    char *data = malloc(datalen);
    while (!feof(file))
    {
        bytesreadend = fread(data + i, 
            sizeof(char), 1024, file
        );
        i += bytesreadend;
        datalen += bytesreadend;
        data = realloc(data, datalen);
    }
    data[i] = (char) 0;

    fclose(file);

    // write datalen
    command = datalen;
    CHECK_BADVAL_RETURN(
        write(socket_fd, &command, sizeof(server_command_t)), 
        -1, -1
    )

    server_command_t res;
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    )
    if (res & WRBACK_FLAG) // server evicted a file
    {
        // write back file to dir..
        
        
    }
    // write data
    CHECK_BADVAL_RETURN(
        write(socket_fd, data, datalen), 
        -1, -1
    )
    free(data);

    // read result of writeFile
    CHECK_BADVAL_RETURN(
    read(socket_fd, &res, sizeof(server_command_t)), 
    -1, -1
    )

    return res;
}
int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname)
{
    if (!socket_fd) return NOTCONN_FLAG;

    server_command_t command = APPEND_OP;
    command |= O_LOCK_FLAG;
    if (dirname != NULL) command |= O_DIR_FLAG;

    unsigned int pathname_len = strlen(pathname);
    if (pathname_len > MESSAGE_MAX_LEN) return -1;
    pathname_len <<= MESSG_SHIFT;

    command |= pathname_len;

    // write command
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    // write pathname
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)pathname, pathname_len * sizeof(char)), 
        -1, -1
    )

    server_command_t res;
    // read result of appendFile 
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    )
    if (res & WRBACK_FLAG) // server evicted a file
    {
        // write back file to dir..
    
        // read result of appendFile
        CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
        )
    }

    return res;
}
int lockFile(const char* pathname)
{
    if (!socket_fd) return NOTCONN_FLAG;

    server_command_t command = LOCK_OP;
    command |= O_LOCK_FLAG;

    unsigned int pathname_len = strlen(pathname);
    if (pathname_len > MESSAGE_MAX_LEN) return -1;
    pathname_len <<= MESSG_SHIFT;

    command |= pathname_len;

    // write command
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    // write pathname
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)pathname, pathname_len * sizeof(char)), 
        -1, -1
    )

    server_command_t res;
    // read result of appendFile 
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    )

    return res;
}
int unlockFile(const char* pathname)
{
    if (!socket_fd) return NOTCONN_FLAG;

    server_command_t command = UNLOCK_OP;

    unsigned int pathname_len = strlen(pathname);
    if (pathname_len > MESSAGE_MAX_LEN) return -1;
    pathname_len <<= MESSG_SHIFT;

    command |= pathname_len;

    // write command
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    // write pathname
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)pathname, pathname_len * sizeof(char)), 
        -1, -1
    )

    server_command_t res;
    // read result of appendFile 
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    )

    return res;
}
int closeFile(const char* pathname)
{
    if (!socket_fd) return NOTCONN_FLAG;

    server_command_t command = CLOSE_OP;

    unsigned int pathname_len = strlen(pathname);
    if (pathname_len > MESSAGE_MAX_LEN) return -1;
    pathname_len <<= MESSG_SHIFT;

    command |= pathname_len;

    // write command
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    // write pathname
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)pathname, pathname_len * sizeof(char)), 
        -1, -1
    )

    server_command_t res;
    // read result of appendFile 
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    );

    return res;
}
int removeFile(const char* pathname)
{
    if (!socket_fd) return NOTCONN_FLAG;

    server_command_t command = REMOVE_OP;

    unsigned int pathname_len = strlen(pathname);
    if (pathname_len > MESSAGE_MAX_LEN) return -1;
    pathname_len <<= MESSG_SHIFT;

    command |= pathname_len;

    // write command
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    // write pathname
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)pathname, pathname_len * sizeof(char)), 
        -1, -1
    )

    server_command_t res;
    // read result of appendFile 
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    );

    return 0;
}
