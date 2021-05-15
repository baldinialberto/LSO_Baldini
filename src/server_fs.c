#include <server_fs.h>

server_fs init_server_fs(int maxCapacity, int maxFileCount)
{
    server_fs fs = {
        maxFileCount, maxCapacity, 
        NULL, 0, NULL, NULL
    };

    CHECK_BADVAL_PERROR_EXIT(
        fs.root = calloc(1, sizeof(server_fs_node)), 
        NULL, "init_server_fs"
    );
    fs.root->inode |= SFS_DIR;


    return fs;
}

int sfs_create(server_fs *fs, char *path, int size, void *buff)
{
    if (fs == NULL || path == NULL || buff == NULL || size <= 0)
        return -1;
    
    const int pathlen = strlen(path);
    char pathtok[pathlen + 1];    
    char *saveptr, *token;
    
    strcpy(pathtok, path); // don't corrupt arg path

    while (token = strtok_r(pathtok, "/", &saveptr), token != NULL)
    {
        
    }

    return 0;
}
