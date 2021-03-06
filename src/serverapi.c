#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <serverapi.h>
#include <server_message.h>
#include <mem_utils.h>
#include <file_utils.h>
#include <string_utils.h>

int server_socket_fd;

int sapi_evict(const char* dirname)
{
	puts(__func__);
	printf("dirname = %s\n", dirname);
	u_string destpath = su_string_from_literal("");
	char *path = NULL;
	void* tempdata = NULL;
	size_t tempdatalen = 0;
	if (sapi_senddatalen(1))
	{
		fprintf(stdout, "writeFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	while (sapi_get_response() == SAPI_EVICT)
	{
		if (sapi_get_path(&path))
		{
			fprintf(stdout, "writeFile : sapi_get_data returned an error\n");
			fflush(stdout);
			return -1;
		}
		printf("path = [%s]\n", path);
		if (su_append_chars(&destpath, dirname))
		{
			fprintf(stdout, "writeFile : su_append_literal returned an error\n");
			fflush(stdout);
			su_free_string(&destpath);
			continue;
		}
		if (su_append_chars(&destpath, "/"))
		{
			fprintf(stdout, "writeFile : su_append_literal returned an error\n");
			fflush(stdout);
			su_free_string(&destpath);
			continue;
		}
		if (su_append_chars(&destpath, path))
		{
			fprintf(stdout, "writeFile : su_append_literal returned an error\n");
			fflush(stdout);
			su_free_string(&destpath);
			continue;
		}

		printf("%s, file to evict = %s\n", __func__, destpath.data);
		// get filedata
		if (sapi_get_data(&tempdata, &tempdatalen))
		{
			fprintf(stdout, "writeFile : sapi_senddata returned an error\n");
			fflush(stdout);
		}
		else
		{
			// write down filedata to destpath
			if (fu_write_to_path(destpath.data, tempdata, tempdatalen) == -1)
			{
				fprintf(stdout, "writeFile : sapi_senddata returned an error\n");
				fflush(stdout);
			}
		}
		su_realloc(&destpath, 0);
	}
	su_free_string(&destpath);
	mu_free(tempdata);
	mu_free(path);
	return 0;
}
void sapi_printerror(FILE* fstream, s_message message)
{
	puts(__func__);
	if (fstream == NULL)
	{
		fprintf(stdout, "sapi_printerror : param fstream == NULL\n");
		fflush(stdout);
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
	puts(__func__);
	if (!server_socket_fd)
	{
		fprintf(stdout, "sapi_sendop : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	s_message message = SAPI_MESSAGE_LEN(flags, op, messlen);
	if (write(server_socket_fd, &message, sizeof(s_message)) == -1)
	{
		fprintf(stdout, "sapi_sendop : write returned an error\n");
		fflush(stdout);
		perror("sapi_sendop : write");
		return -1;
	}
	return 0;
}
int sapi_senddatalen(size_t datalen)
{
	puts(__func__);
	if (!server_socket_fd)
	{
		fprintf(stdout, "sapi_senddatalen : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	if (write(server_socket_fd, &datalen, sizeof(size_t)) == -1)
	{
		fprintf(stdout, "sapi_senddatalen : write returned an error\n");
		fflush(stdout);
		perror("sapi_senddata : write");
		return -1;
	}
	return 0;
}
int sapi_senddata(void* data, size_t datalen)
{
	puts(__func__);
	if (data == NULL)
	{
		fprintf(stdout, "sapi_senddata : param data == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (datalen == 0)
	{
		fprintf(stdout, "sapi_senddata : param datalen == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "sapi_senddata : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	if (write(server_socket_fd, data, datalen) == -1)
	{
		fprintf(stdout, "sapi_senddata : write returned an error\n");
		fflush(stdout);
		perror("sapi_senddata : write");
		return -1;
	}
	return 0;
}
s_message sapi_get_response()
{
	puts(__func__);
	s_message message;
	if (read(server_socket_fd, &message, sizeof(s_message)) == -1)
	{
		fprintf(stdout, "sapi_getresponse : read returned an error\n");
		fflush(stdout);
		perror("sapi_getresponse : read");
		return -1;
	}
	return message;
}
int sapi_get_path(char **path)
{
	puts(__func__);
	if (path == NULL)
	{
		fprintf(stdout, "%s : param buff == NULL\n", __func__);
		fflush(stdout);

		return -1;
	}
	size_t path_len = 0;
	if (read(server_socket_fd, &path_len, sizeof(size_t)) == -1)
	{
		perror("read at sapi_getpath");
		mu_free(*path);
		return -1;
	}
	fprintf(stdout, "%s, path_len = %zu\n", __func__, path_len);
	if (path_len == 0)
	{
		return -1;
	}
	*path = mu_realloc(*path, path_len + 1);
	memset(*path + path_len, 0,  1);
	if (read(server_socket_fd, *path, path_len) == -1)
	{
		perror("read at sapi_getpath");
		mu_free(*path);
		return -1;
	}

	printf("path = %s\n", *path);

	return 0;
}
int sapi_get_data(void** data, size_t* datalen)
{
	puts(__func__);
	if (data == NULL)
	{
		fprintf(stdout, "sapi_get_data : param buff == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (datalen == NULL)
	{
		fprintf(stdout, "sapi_get_data : param size == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (read(server_socket_fd, datalen, sizeof(size_t)) == -1)
	{
		fprintf(stdout, "sapi_get_data : read returned an error\n");
		fflush(stdout);
		perror("sapi_get_data : read");
		return -1;
	}
	if (*datalen > 0)
	{
		printf("mu_realloc size = %zu\n", *datalen);
		*data = mu_realloc(*data, *datalen);
		memset(*data, 0, *datalen);
		if (read(server_socket_fd, *data, *datalen) == -1)
		{
			fprintf(stdout, "sapi_get_data : read returned an error\n");
			fflush(stdout);
			perror("sapi_get_data : read");
			return -1;
		}
	}
	else
	{
		// no data to read
		return -1;
	}

	return 0;
}
int openConnection(const char* sockname, int msec, const struct timespec abstime)
{
	puts(__func__);
	if (sockname == NULL)
	{
		fprintf(stdout, "openConnection : param sockname == NULL\n");
		fflush(stdout);
		return -1;
	}

	struct sockaddr_un sa;
	strncpy(sa.sun_path, sockname, sizeof(sa.sun_path) - 1);
	sa.sun_family = AF_UNIX;

	if ((server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stdout, "openConnection : socket returned -1\n");
		fflush(stdout);
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
	while (connect(server_socket_fd, (struct sockaddr*)&sa, sizeof(sa)) == -1 &&
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
			return (-1);
		}
	}

	if (remaining_time > 0) return 0;

	fprintf(stdout, "Not connected\n");
	return -1;
}
int closeConnection(const char* sockname)
{
	puts(__func__);
	if (sockname == NULL)
	{
		fprintf(stdout, "closeConnection : param sockname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "closeConnection : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	return close(server_socket_fd);
}
int openFile(const char* pathname, int flags)
{
	puts(__func__);
	if (pathname == NULL)
	{
		fprintf(stdout, "openFile : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "openFile : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	fprintf(stdout, "openFile pathname = %s, flags = %X\n", pathname, flags);
	fflush(stdout);

	if (sapi_sendop(strlen(pathname), SAPI_OPENFILE, (unsigned char)flags) == -1)
	{
		fprintf(stdout, "openFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
	{
		fprintf(stdout, "openFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	s_message m = sapi_get_response();
	if (m != SAPI_SUCCESS)
	{
		fprintf(stdout, "openFile : sapi_getresponse returned an error\n");
		fflush(stdout);
		sapi_printerror(stdout, m);
		return -1;
	}
	return 0;
}
int readFile(const char* pathname, void** buf, size_t* size)
{
	puts(__func__);
	if (pathname == NULL)
	{
		fprintf(stdout, "readFile : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (buf == NULL)
	{
		fprintf(stdout, "readFile : param buf == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (size == NULL)
	{
		fprintf(stdout, "readFile : param size == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "readFile : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	fprintf(stdout, "readFile pathname = %s\n", pathname);
	fflush(stdout);
	// send request to open file
	if (sapi_sendop(strlen(pathname), (unsigned char)SAPI_READFILE, (unsigned char)0) == -1)
	{
		fprintf(stdout, "readFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	// send filename
	if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
	{
		fprintf(stdout, "readFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	// receive confirmation
	s_message m;
	if ((m = sapi_get_response()))
	{
		fprintf(stdout, "readFile : sapi_getresponse returned an error\n");
		fflush(stdout);
		sapi_printerror(stdout, m);
		return -1;
	}
	// receive data
	if (sapi_get_data(buf, size) == -1)
	{
		fprintf(stdout, "readFile : sapi_get_data returned an error\n");
		fflush(stdout);
		return -1;
	}
	return 0;
}
int readNFiles(unsigned int N, const char* dirname)
{
	puts(__func__);
	if (N == 0)
	{
		fprintf(stdout, "readNFiles : param N == 0 -> nothing to do\n");
		fflush(stdout);
		return 0;
	}
	if (dirname == NULL)
	{
		fprintf(stdout, "readNFiles : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "readNFiles : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	// ask to read N files
	if (sapi_sendop(N, (unsigned char)SAPI_READNFILES, (unsigned char)0) == -1)
	{
		fprintf(stdout, "readFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	int nfiles_readen = 0;
	void* buff = NULL;
	char *path = NULL;
	size_t size = 0;
	u_string destpath = su_string_from_literal("");
	while (1) // get filename
	{
		if (sapi_get_path(&path))
		{
			fprintf(stdout, "readNFiles : su_append_literal returned an error\n");
			fflush(stdout);
			perror("sapi_get_path");
			exit(1);
		}
		if (path == NULL || strlen(path) == 0)
		{
			mu_free(buff);
			mu_free(path);
			su_free_string(&destpath);

			return nfiles_readen;
		}
		// there's data to read
		// create destpath  = dirname + filename
		if (su_append_chars(&destpath, dirname))
		{
			fprintf(stdout, "readNFiles : su_append_literal returned an error\n");
			fflush(stdout);
			su_free_string(&destpath);
			continue;
		}
		if (su_append_chars(&destpath, path))
		{
			fprintf(stdout, "readNFiles : su_append_literal returned an error\n");
			fflush(stdout);
			su_free_string(&destpath);
			continue;
		}
		// get filedata
		if (sapi_get_data(&buff, &size) == -1)
		{
			fprintf(stdout, "readNFiles : sapi_senddata returned an error\n");
			fflush(stdout);
		}
		else
		{
			// write down filedata to destpath
			if (fu_write_to_path(destpath.data, buff, size) == -1)
			{
				fprintf(stdout, "readNFiles : sapi_senddata returned an error\n");
				fflush(stdout);
			}
			else
			{
				nfiles_readen++;
			}
		}
		su_realloc(&destpath, 0);
		mu_free(path);
		mu_free(buff);
		path = NULL;
		buff = NULL;
	}

}
int writeFile(const char* pathname, const char* dirname)
{
	puts(__func__);
	void* data = NULL;
	size_t datalen = 0;
	s_message m;

	if (pathname == NULL)
	{
		fprintf(stdout, "writeFile : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "readNFiles : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	fprintf(stdout, "writeFile pathname = %s\n", pathname);
	fflush(stdout);
	if (fu_read_from_path(pathname, &data, &datalen) == -1)
	{
		mu_free(data);
		fprintf(stdout, "writeFile : fu_readpath returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_sendop(strlen(pathname), SAPI_WRITEFILE, (unsigned char)0) == -1)
	{
		mu_free(data);
		fprintf(stdout, "writeFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
	{
		mu_free(data);
		fprintf(stdout, "writeFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddatalen(datalen) == -1)
	{
		fprintf(stdout, "writeFile : sapi_senddatalen returned an error\n");
		fflush(stdout);
		return -1;
	}
	m = sapi_get_response();
	if (m == SAPI_EVICT)
	{
		if (sapi_senddatalen(dirname != NULL))
		{
			mu_free(data);
			fprintf(stdout, "writeFile : sapi_sendop returned an error\n");
			fflush(stdout);
			return -1;
		}
		if (dirname != NULL && sapi_evict(dirname) == -1)
		{
			mu_free(data);
			fprintf(stdout, "writeFile : sapi_evict returned an error\n");
			fflush(stdout);
			return -1;
		}
	}
	if (m != SAPI_SUCCESS)
	{
		mu_free(data);
		sapi_printerror(stdout, m);
		return -1;
	}
	if (sapi_senddata(data, datalen) == -1)
	{
		mu_free(data);
		fprintf(stdout, "writeFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	mu_free(data);
	if ((m = sapi_get_response()))
	{
		fprintf(stdout, "writeFile : sapi_getresponse returned an error\n");
		fflush(stdout);
		sapi_printerror(stdout, m);
		return -1;
	}
	return 0;
}
int appendToFile(const char* pathname, void* buf, size_t size, const char* dirname)
{
	puts(__func__);
	s_message m;
	if (pathname == NULL)
	{
		fprintf(stdout, "appendToFile : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (buf == NULL && size != 0)
	{
		fprintf(stdout, "appendToFile : param buf == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (size == 0 && buf != NULL)
	{
		fprintf(stdout, "appendToFile : param size == 0\n");
		fflush(stdout);
		return -1;
	}
	if (size == 0 && buf == NULL)
	{
		fprintf(stdout, "appendToFile : param buf == NULL && size == 0 -> nothing to do\n");
		fflush(stdout);
		return 0;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "appendToFile : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_sendop(strlen(pathname), SAPI_APPENDFILE, 0) == -1)
	{
		fprintf(stdout, "appendToFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddata((void*)pathname, strlen(pathname)))
	{
		fprintf(stdout, "appendToFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddatalen(size) == -1)
	{
		fprintf(stdout, "appendToFile : sapi_senddatalen returned an error\n");
		fflush(stdout);
		return -1;
	}
	if ((m = sapi_get_response()))
	{
		if (m != SAPI_EVICT)
		{
			fprintf(stdout, "appendToFile : sapi_getresponse returned an error\n");
			fflush(stdout);
			sapi_printerror(stdout, m);
			return -1;
		}
		if (dirname == NULL) // don't save evicted files
		{
			if (sapi_senddatalen(0))
			{
				fprintf(stdout, "appendToFile : sapi_sendop returned an error\n");
				fflush(stdout);
				return -1;
			}
		}
		else    // save evicted files
		{
			if (sapi_evict(dirname) == -1)
			{
				fprintf(stdout, "appendToFile : sapi_evict returned an error\n");
				fflush(stdout);
				return -1;
			}
		}
	}
	if (sapi_senddata(buf, size) == -1)
	{
		fprintf(stdout, "writeFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	if ((m = sapi_get_response()))
	{
		fprintf(stdout, "writeFile : sapi_getreponse returned an error\n");
		fflush(stdout);
		sapi_printerror(stdout, m);
		return -1;
	}
	return 0;
}
int lockFile(const char* pathname)
{
	puts(__func__);
	if (pathname == NULL)
	{
		fprintf(stdout, "lockFile : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "lockFile : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_sendop(strlen(pathname), SAPI_LOCKFILE, (unsigned char)0) == -1)
	{
		fprintf(stdout, "lockFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
	{
		fprintf(stdout, "lockFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	s_message m;
	if ((m = sapi_get_response()))
	{
		fprintf(stdout, "lockFile : sapi_getresponse returned an error\n");
		fflush(stdout);
		sapi_printerror(stdout, m);
		return -1;
	}
	return 0;
}
int unlockFile(const char* pathname)
{
	puts(__func__);
	if (pathname == NULL)
	{
		fprintf(stdout, "unlockFile : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "unlockFile : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_sendop(strlen(pathname), SAPI_UNLOCKFILE, (unsigned char)0) == -1)
	{
		fprintf(stdout, "unlockFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
	{
		fprintf(stdout, "unlockFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	s_message m;
	if ((m = sapi_get_response()))
	{
		fprintf(stdout, "unlockFile : sapi_getresponse returned an error\n");
		fflush(stdout);
		sapi_printerror(stdout, m);
		return -1;
	}
	return 0;
}
int closeFile(const char* pathname)
{
	puts(__func__);
	if (pathname == NULL)
	{
		fprintf(stdout, "closeFile : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "closeFile : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	fprintf(stdout, "closeFile pathname = %s\n", pathname);
	fflush(stdout);
	if (sapi_sendop(strlen(pathname), SAPI_CLOSEFILE, (unsigned char)0) == -1)
	{
		fprintf(stdout, "closeFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
	{
		fprintf(stdout, "closeFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	s_message m;
	if ((m = sapi_get_response()))
	{
		fprintf(stdout, "closeFile : sapi_getresponse returned an error\n");
		fflush(stdout);
		sapi_printerror(stdout, m);
		return -1;
	}
	return 0;
}
int removeFile(const char* pathname)
{
	puts(__func__);
	if (pathname == NULL)
	{
		fprintf(stdout, "removeFile : param pathname == NULL\n");
		fflush(stdout);
		return -1;
	}
	if (!server_socket_fd)
	{
		fprintf(stdout, "removeFile : socket_fd == 0 -> client not connected\n");
		fflush(stdout);
		return -1;
	}
	fprintf(stdout, "removeFile pathname = %s\n", pathname);
	fflush(stdout);
	if (sapi_sendop(strlen(pathname), SAPI_REMOVEFILE, (unsigned char)0) == -1)
	{
		fprintf(stdout, "removeFile : sapi_sendop returned an error\n");
		fflush(stdout);
		return -1;
	}
	if (sapi_senddata((void*)pathname, strlen(pathname)) == -1)
	{
		fprintf(stdout, "removeFile : sapi_senddata returned an error\n");
		fflush(stdout);
		return -1;
	}
	s_message m;
	if ((m = sapi_get_response()))
	{
		fprintf(stdout, "removeFile : sapi_getresponse returned an error\n");
		fflush(stdout);
		sapi_printerror(stdout, m);
		return -1;
	}
	return 0;
}
