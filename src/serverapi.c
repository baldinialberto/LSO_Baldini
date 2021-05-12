#include <serverapi.h>

static int socket_fd;

int openConnection(const char* sockname, int msec, const struct timespec abstime)
{
    struct sockaddr_un sa;
	strncpy(sa.sun_path, sockname, sizeof(sa.sun_path)-1);
	sa.sun_family = AF_UNIX;

    CHECK_BADVAL_PERROR_RETURN(
        socket_fd = socket(AF_UNIX, SOCK_STREAM, 0), 
        -1, "openConnection : socket", NOTCONN_FLAG
    )

    const struct timespec inter_requests_time = {0, (long)msec * 1000};
    assert(inter_requests_time.tv_nsec == (long)msec * 1000);
    struct timespec tmp = {0};
    struct timespec remaining_time = abstime;
    
    while (connect(socket_fd, (struct sockaddr *)&sa, sizeof(sa)) == -1 && 
    remaining_time.tv_nsec > 0)
	{
		if (errno == ENOENT)
        {
			nanosleep(&inter_requests_time, &tmp);
            remaining_time.tv_nsec -= (inter_requests_time.tv_nsec - tmp.tv_sec);
            memset(&tmp, 0, sizeof(struct timespec));
        }
		else
			exit(EXIT_FAILURE);
        errno = 0;
	}

    return NOTCONN_FLAG | FNF_FLAG;
}
int closeConnection(const char* sockname)
{
    if (!socket_fd) return NOTCONN_FLAG | ALREADY_FLAG;

    server_command_t command = CLSCONN_OP;
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )

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
    if (res != (server_command_t)0) return res;

    res >>= MESSG_SHIFT; // len of chars readen

    *buf = calloc(res, sizeof(char*));
    *size = (size_t)res;

    CHECK_BADVAL_RETURN(
        read(socket_fd, *buf, res), 
        -1, -1
    )

    return res;
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

    server_command_t res;
    // read result of appendFile 
    CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
    )
    if (res & WRBACK_FLAG) // server evicted a file
    {
        // write back file to dir..
    
        // read result of writeFile
        CHECK_BADVAL_RETURN(
        read(socket_fd, &res, sizeof(server_command_t)), 
        -1, -1
        )
    }

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