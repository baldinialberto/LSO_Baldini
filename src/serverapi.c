#include <serverapi.h>

static int socket_fd;

int openConnection(const char* sockname, int msec, const struct timespec abstime)
{
    return 0;
}
int closeConnection(const char* sockname)
{
    server_command_t command = CLSCONN_OP;
    CHECK_BADVAL_RETURN(
        write(socket_fd, (void *)&command, sizeof(server_command_t)), 
        -1, -1
    )
    return close(socket_fd);
}
int openFile(const char* pathname, int flags)
{
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
    if (res & WRBACK_OP) // server evicted a file
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
    if (res & WRBACK_OP) // server evicted a file
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