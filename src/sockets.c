#include "../include/sockets.h"

void socket_errno_check(const char* source)
{
    perror(source);
    if (errno & EACCES)
    {
        // Permission to create a socket of the specified type and/or protocol is denied.
    }
    if (errno & EAFNOSUPPORT)
    {
        // The implementation does not support the specified address family.
    }
    if (errno & EINVAL)
    {
        // Invalid flags in type.
    }
    if (errno & EMFILE)
    {
        // The per-process limit on the number of open file descriptors has been reached.
    }
    if (errno & ENFILE)
    {
        // The system - wide limit on the total number of open files has been reached.
    }
    if (errno & (ENOBUFS | ENOMEM))
    {
        // The system-wide limit on the total number of open files has been reached.
    }
    if (errno & EPROTONOSUPPORT)
    {
        // The protocol type or the specified protocol is not supported within this domain.
    } 
}

int create_server_socket(const char* socketName, int queueLen)
{
    int new_socket;

    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path,
        socketName, 
        sizeof(server_addr.sun_path) - 1
    );

    if ((new_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        socket_errno_check("create_server_socket");
    }
    else 
    {
        CHECK_BADVAL_PERROR(
            bind(new_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)), 
            -1, 
            "Bind ServerSocket"
        );
        CHECK_BADVAL_PERROR(
            listen(new_socket, queueLen), 
            -1, 
            "Listen ServerSocket"
        );
        
        // arrivato qui

        return new_socket;
    }

    return -1;
}
int create_client_socket()
{
    int new_socket;
    if ((new_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        socket_errno_check("create_server_socket");
    }
    return new_socket;
}
int connect_to_server_socket(const char* socketName)
{

}
int socket_write(int sock_fd, void* buff, size_t len)
{

}
int socket_read(int sock_fd, void* buff, size_t size)
{

}