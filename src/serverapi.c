#include "../include/serverapi.h"

int server_socket_fd;

int sapi_evict(const char *dirname)
{
    u_string destpath = su_string_from_literal("");
    void *tempdata = NULL;
    size_t tempdatalen = 0;
    if (sapi_senddatalen(1))
    {
        fprintf(stderr, "writeFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    while (sapi_getresponse() == SAPI_SUCCESS)
    {
        // there's a file to evict
        // get filename
        if (sapi_getdata(&tempdata, &tempdatalen))
        {
            fprintf(stderr, "writeFile : sapi_getdata returned an error\n");
            fflush(stderr);
            return -1;
        }
        // create destpath  = dirname + filename 
        if (su_append_chars(&destpath, dirname))
        {
            fprintf(stderr, "writeFile : su_append_literal returned an error\n");
            fflush(stderr);
            su_free_string(&destpath);
            continue;
        }
        if (su_append_chars(&destpath, (char *)tempdata))
        {
            fprintf(stderr, "writeFile : su_append_literal returned an error\n");
            fflush(stderr);
            su_free_string(&destpath);
            continue;
        }
        // get filedata
        if (sapi_getdata(&tempdata, &tempdatalen) == -1)
        {
            fprintf(stderr, "writeFile : sapi_senddata returned an error\n");
            fflush(stderr);
        }
        else
        {
            // write down filedata to destpath
            if (fu_writepath(destpath.data, tempdata, tempdatalen) == -1)
            {
                fprintf(stderr, "writeFile : sapi_senddata returned an error\n");
                fflush(stderr);
            }
        }
        su_realloc(&destpath, 0);
    }
    su_free_string(&destpath);
    return 0;
}
void sapi_printerror(FILE *fstream, s_message message)
{
    if (fstream == NULL)
    {
        fprintf(stderr, "sapi_printerror : param fstream == NULL\n");
        fflush(stderr);
        return;
    }
    if (message == 0)
    {
        fprintf(stdout, "sapi_printerror : no error\n");
        fflush(stdout);
        return;
    }
    switch (message)
    {
        case SAPI_EVICT:
            fprintf(fstream, "sapi : server evict\n");
            fflush(fstream);
            break;
        case SAPI_FNF:
            fprintf(fstream, "sapi : file not found\n");
            fflush(fstream);
            break;
        default: // SAPI_FAILURE
            fprintf(fstream, "sapi : server error\n");
            fflush(fstream);
            break;
    }
}
int sapi_sendop(unsigned int messlen, unsigned char op, unsigned char flags)
{
    if (!server_socket_fd)
    {
        fprintf(stderr, "sapi_sendop : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    s_message message = SAPI_MESSAGE_LEN(flags, op, messlen);
    if (write(server_socket_fd, &message, sizeof(s_message)) == -1)
    {
        fprintf(stderr, "sapi_sendop : write returned an error\n");
        fflush(stderr);
        perror("sapi_sendop : write");
        return -1;
    }
    return 0;
}
int sapi_senddatalen(size_t datalen)
{
    if (!server_socket_fd)
    {
        fprintf(stderr, "sapi_senddatalen : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    if (write(server_socket_fd, &datalen, sizeof(size_t)) == -1)
    {
        fprintf(stderr, "sapi_senddatalen : write returned an error\n");
        fflush(stderr);
        perror("sapi_senddata : write");
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
    return 0;
}
s_message sapi_getresponse()
{
    s_message message;
    if (read(server_socket_fd, &message, sizeof(s_message)) == -1)
    {
        fprintf(stderr, "sapi_getresponse : read returned an error\n");
        fflush(stderr);
        perror("sapi_getresponse : read");
        return -1;
    }
    return message;
}
int sapi_getdata(void **buff, size_t *size)
{
    if (buff == NULL)
    {
        fprintf(stderr, "sapi_getdata : param buff == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (size == NULL)
    {
        fprintf(stderr, "sapi_getdata : param size == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (read(server_socket_fd, size, sizeof(size_t)) == -1)
    {
        fprintf(stderr, "sapi_getdata : read returned an error\n");
        fflush(stderr);
        perror("sapi_getdata : read");
        return -1;
    }
    if (*size > 0)
    {
        *buff = mu_realloc(*buff, *size);
        if (read(server_socket_fd, *buff, *size) == -1)
        {
            fprintf(stderr, "sapi_getdata : read returned an error\n");
            fflush(stderr);
            perror("sapi_getdata : read");
            return -1;
        }
    } else 
    {
        // no data to read
        return -1;
    }

    return 0;
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

    if ((server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        fprintf(stderr, "openConnection : socket returned -1\n");
        fflush(stderr);
        perror("openConnection : socket");
        return -1;
    }

    struct timespec inter_requests_time;
    long int remaining_time;
    inter_requests_time.tv_sec = msec / 1000;
    inter_requests_time.tv_nsec = (msec % 1000) * 1000000;
    if (inter_requests_time.tv_nsec > 999999999 || inter_requests_time.tv_nsec < 0)
    {
        inter_requests_time.tv_nsec = 500000000;
    }
    remaining_time = abstime.tv_nsec + (abstime.tv_sec * 1000000000L);
    while (connect(server_socket_fd, (struct sockaddr *)&sa, sizeof(sa)) == -1 && 
    remaining_time > 0)
	{
		if (errno == ENOENT || errno == ECONNREFUSED) 
        {
			nanosleep(&inter_requests_time, NULL);
            remaining_time -= (inter_requests_time.tv_nsec + inter_requests_time.tv_sec * 1000000000L);
        }
		else
        {
            perror("openConnection : connect");
			return(-1);
        }
	}

    if (remaining_time > 0) return 0;

    fprintf(stdout, "Not connected\n");
    return -1;
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
    if (sapi_sendop(strlen(pathname), SAPI_OPENFILE, (unsigned char)flags) == -1)
    {
        fprintf(stderr, "openFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddata((void *)pathname, strlen(pathname)) == -1)
    {
        fprintf(stderr, "openFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    s_message m;
    if ((m = sapi_getresponse()))
    {
        fprintf(stderr, "openFile : sapi_getresponse returned an error\n");
        fflush(stderr);
        sapi_printerror(stderr, m);
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
    // send request to open file
    if (sapi_sendop(strlen(pathname), (unsigned char) SAPI_READFILE, (unsigned char) 0) == -1)
    {
        fprintf(stderr, "readFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    // send filename
    if (sapi_senddata((void *)pathname, strlen(pathname)) == -1)
    {
        fprintf(stderr, "readFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    // receive confirmation
    s_message m;
    if ((m = sapi_getresponse()))
    {
        fprintf(stderr, "readFile : sapi_getresponse returned an error\n");
        fflush(stderr);
        sapi_printerror(stderr, m);
        return -1;
    }
    // receive data
    if (sapi_getdata(buf, size) == -1)
    {
        fprintf(stderr, "readFile : sapi_getdata returned an error\n");
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
    // ask to read N files
    if (sapi_sendop(N, (unsigned char) SAPI_READNFILES, (unsigned char) 0) == -1)
    {
        fprintf(stderr, "readFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    int nfiles_readen = 0;
    void *buff = NULL;
    size_t size = 0;
    u_string destpath = su_string_from_literal("");
    while (sapi_getdata(&buff, &size) != -1) // get filename
    {
        // there's data to read
        // create destpath  = dirname + filename 
        if (su_append_chars(&destpath, dirname))
        {
            fprintf(stderr, "readNFiles : su_append_literal returned an error\n");
            fflush(stderr);
            su_free_string(&destpath);
            continue;
        }
        if (su_append_chars(&destpath, (char *)buff))
        {
            fprintf(stderr, "readNFiles : su_append_literal returned an error\n");
            fflush(stderr);
            su_free_string(&destpath);
            continue;
        }
        // get filedata
        if (sapi_getdata(&buff, &size) == -1)
        {
            fprintf(stderr, "readNFiles : sapi_senddata returned an error\n");
            fflush(stderr);
        }
        else
        {
            // write down filedata to destpath
            if (fu_writepath(destpath.data, buff, size) == -1)
            {
                fprintf(stderr, "readNFiles : sapi_senddata returned an error\n");
                fflush(stderr);
            }
            else 
            {
                nfiles_readen++;
            }
        }
        su_realloc(&destpath, 0);
    }
    mu_free(buff);
    su_free_string(&destpath);

    return nfiles_readen;
}
int writeFile(const char* pathname, const char* dirname)
{
    void *data = NULL;
    size_t datalen = 0;
    s_message m;
    
    if (pathname == NULL)
    {
        fprintf(stderr, "writeFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!server_socket_fd)
    {
        fprintf(stderr, "readNFiles : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    if (fu_readpath(pathname, &data, &datalen) == -1)
    {
        fprintf(stderr, "writeFile : fu_readpath returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_sendop(strlen(pathname), SAPI_WRITEFILE, (unsigned char) 0) == -1)
    {
        fprintf(stderr, "writeFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
    {
        fprintf(stderr, "writeFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddatalen(datalen) == -1)
    {
        fprintf(stderr, "writeFile : sapi_senddatalen returned an error\n");
        fflush(stderr);
        return -1;
    }
    if ((m = sapi_getresponse()))
    {
        if (m != SAPI_EVICT) {
            fprintf(stderr, "writeFile : sapi_getresponse returned an error\n");
            fflush(stderr);
            sapi_printerror(stderr, m);
            return -1;
        }
        if (dirname != NULL) // don't save evicted files
        {
            if (sapi_senddatalen(0))
            {
                fprintf(stderr, "writeFile : sapi_sendop returned an error\n");
                fflush(stderr);
                return -1;
            }
        }
        else    // save evicted files
        {
            if (sapi_evict(dirname) == -1)
            {
                fprintf(stderr, "writeFile : sapi_evict returned an error\n");
                fflush(stderr);
                return -1;
            }
        }
        
    }
    if (sapi_senddata(data, datalen) == -1)
    {
        fprintf(stderr, "writeFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    if ((m = sapi_getresponse()))
    {
        fprintf(stderr, "writeFile : sapi_getreponse returned an error\n");
        fflush(stderr);
        sapi_printerror(stderr, m);
        return -1;
    }
    return 0;
}
int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname)
{
    s_message m;
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
    if (sapi_sendop(strlen(pathname), SAPI_APPENDFILE, 0) == -1)
    {
        fprintf(stderr, "appendToFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddata((void*)pathname, strlen(pathname)))
    {
        fprintf(stderr, "appendToFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddatalen(size) == -1)
    {
        fprintf(stderr, "appendToFile : sapi_senddatalen returned an error\n");
        fflush(stderr);
        return -1;
    }
    if ((m = sapi_getresponse()))
    {
        if (m != SAPI_EVICT) {
            fprintf(stderr, "appendToFile : sapi_getresponse returned an error\n");
            fflush(stderr);
            sapi_printerror(stderr, m);
            return -1;
        }
        if (dirname != NULL) // don't save evicted files
        {
            if (sapi_senddatalen(0))
            {
                fprintf(stderr, "appendToFile : sapi_sendop returned an error\n");
                fflush(stderr);
                return -1;
            }
        }
        else    // save evicted files
        {
            if (sapi_evict(dirname) == -1)
            {
                fprintf(stderr, "appendToFile : sapi_evict returned an error\n");
                fflush(stderr);
                return -1;
            }
        }
    }
    if (sapi_senddata(buf, size) == -1)
    {
        fprintf(stderr, "writeFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    if ((m = sapi_getresponse()))
    {
        fprintf(stderr, "writeFile : sapi_getreponse returned an error\n");
        fflush(stderr);
        sapi_printerror(stderr, m);
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
    if (sapi_sendop(strlen(pathname), SAPI_LOCKFILE, (unsigned char)0) == -1)
    {
        fprintf(stderr, "lockFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
    {
        fprintf(stderr, "lockFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    s_message m;
    if ((m = sapi_getresponse()))
    {
        fprintf(stderr, "lockFile : sapi_getresponse returned an error\n");
        fflush(stderr);
        sapi_printerror(stderr, m);
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
    if (sapi_sendop(strlen(pathname), SAPI_UNLOCKFILE, (unsigned char)0) == -1)
    {
        fprintf(stderr, "unlockFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
    {
        fprintf(stderr, "unlockFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    s_message m;
    if ((m = sapi_getresponse()))
    {
        fprintf(stderr, "unlockFile : sapi_getresponse returned an error\n");
        fflush(stderr);
        sapi_printerror(stderr, m);
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
    if (sapi_sendop(strlen(pathname), SAPI_CLOSEFILE, (unsigned char)0) == -1)
    {
        fprintf(stderr, "closeFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
    {
        fprintf(stderr, "closeFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    s_message m;
    if ((m = sapi_getresponse()))
    {
        fprintf(stderr, "closeFile : sapi_getresponse returned an error\n");
        fflush(stderr);
        sapi_printerror(stderr, m);
        return -1;
    }
    return 0;
}
int removeFile(const char* pathname)
{
    if (pathname == NULL)
    {
        fprintf(stderr, "removeFile : param pathname == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (!server_socket_fd)
    {
        fprintf(stderr, "removeFile : socket_fd == 0 -> client not connected\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_sendop(strlen(pathname), SAPI_REMOVEFILE, (unsigned char)0) == -1)
    {
        fprintf(stderr, "removeFile : sapi_sendop returned an error\n");
        fflush(stderr);
        return -1;
    }
    if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
    {
        fprintf(stderr, "removeFile : sapi_senddata returned an error\n");
        fflush(stderr);
        return -1;
    }
    s_message m;
    if ((m = sapi_getresponse()))
    {
        fprintf(stderr, "removeFile : sapi_getresponse returned an error\n");
        fflush(stderr);
        sapi_printerror(stderr, m);
        return -1;
    }
    return 0;
}
