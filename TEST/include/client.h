#ifndef CLIENT_H
#define CLIENT_H

#include <list_utils.h>

typedef struct client_conf
{
    char *server_socket;
    u_list files_to_write;
    u_list files_to_read;
    u_list files_to_delete;

} client_conf;

/**
 * Parse Client arguments
 * @param argc args count
 * @param argv args vector
 * @return 0 if successful, -1 otherwise
 */
int client_parse_args(int argc, char **argv);


#endif
