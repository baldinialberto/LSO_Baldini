#include <server_fs.h>

int  sfs_init(sfs_fs *server_fs, int maxMB, int maxFiles)
{
    server_fs->currentFiles = 0;
    server_fs->currentMB = 0;
    server_fs->maxFiles = maxFiles;
    server_fs->maxMB = maxMB;
    server_fs->root = NULL;

    return pthread_mutex_init(&(server_fs->lock), NULL);
}

sfs_file *sfs_find(sfs_fs *server_fs, const char *name)
{
    if (server_fs->root == NULL
        || server_fs->root->sub == NULL 
        || name == NULL) return NULL;
    sfs_folder *currDir = server_fs->root;
    sfs_file *currFile = NULL;
    
    char namecopy[strlen(name) + 1];
    strcpy(namecopy, name);
    char *strsrc = namecopy;
    char *saveptr;
    char *temp = NULL;

    while (temp = strtok_r(strsrc, "/", &saveptr), temp != NULL)
    {
        if (*saveptr)
        {
            // look for folder
            DEBUG(printf("folder %s, -c %c\n", temp, *saveptr));
            CHECK_BADVAL_RETURN(
                currDir = currDir->sub, 
                NULL, NULL
            );
            do 
            {
                if (strcmp(temp, currDir->name) > 0)
                {
                    currDir = currDir->next;
                } 
                else if (strcmp(temp, currDir->name) < 0)
                {
                    return NULL;
                } 
                else {
                    break;
                }
            } while (currDir != NULL);
            if (currDir == NULL) return NULL;
        } else 
        {
            // look for file
            DEBUG(printf("file %s -c %c\n", temp, *saveptr));
            CHECK_BADVAL_RETURN(
                currFile = currDir->files, 
                NULL, NULL
            );
            do 
            {
                if (strcmp(temp, currFile->name) > 0)
                {
                    currDir = currFile->next;
                } 
                else if (strcmp(temp, currFile->name) < 0)
                {
                    return NULL;
                } 
                else {
                    return currFile;
                }
            } while (currFile != NULL);
            if (currFile == NULL) return NULL;
        }
        strsrc = NULL;
    }
    
    return NULL;
}

retptr sfs_open(sfs_fs *server_fs, const char *name, const char mode)
{
    if (server_fs == NULL || name == NULL) 
        return (retptr){1, NULL};
    
    sfs_file *file = sfs_find(server_fs, name);
    if (file == NULL)
        // file not fount
        return (retptr){1, NULL};

    pthread_mutex_lock(&(file->lock));

    if (file->finfo & SFS_FD_LOCK)
        // file locked
        return (retptr){1, NULL};
    if (file->finfo & SFS_FD_CLIENTS && mode == 'w')
        // file opened in readmode
        return (retptr){1, NULL};
    if ((file->finfo & SFS_FD_CLIENTS)==0b1111  
        && mode == 'r' || mode == 'R')
        // file opened in readmode by too many clients
        return (retptr){1, NULL};

    if (mode == 'R' || mode == 'w')
    {
        file->finfo |= SFS_FD_LOCK; // lockfile
    }
    file->finfo++; // add client
    file->finfo += 0b00010000; // add usage
    
    pthread_mutex_unlock(&(file->lock));
    sfs_fd *fd = (sfs_fd *)malloc(sizeof(sfs_fd));
    fd->file = file;
    fd->i = 0;

    return (retptr){0, fd};
}