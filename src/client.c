#include "../include/client.h"

int main(int argc, char** argv)
{	
	client_conf conf = {0};
	parseargs(argc, argv, &conf);
    printargs(&conf);
	int connected = 1;

	if (openConnection(conf.server_socket_filename, conf.connection_timer, 
		(struct timespec){0, 500000000L}))
	{
		perror("openConnection");
		connected = 0;
	}
	
	//write_nfiles_from_dir(conf.folder_to_write, conf.folder_filecount, "");

	if (connected && closeConnection(conf.server_socket_filename))
	{
		perror("closeConnection");
	}

	client_conf_cleanup(&conf);	

	return 0;
}
int parseargs(int argc, char ** argv, client_conf *conf)
{
	int opt;
	const char *flags = "hf:w:W:D:r:R:d:t:l:u:c:p";

	while ((opt = getopt(argc, argv, flags)) != -1)
	{
		switch (opt)
		{
			case 'h':
				fprintf(stdout, "Usage: %s :\n\
[-h stampa la lista di tutte le opzioni accettate dal client e termina immediatamente] \n\
[-f filename : specifica il nome del socket AF_UNIX a cui connettersi]\n\
[-w dirname[,n=0] : invia al server i file nella cartella ‘dirname’]\n\
[-W file1[,file2]: lista di nomi di file da scrivere nel server separati da ‘,’]\n\
[-D dirname : cartella in memoria secondaria dove vengono scritti i file che il server rimuove a seguito di capacity misses]\n\
[-r file1[,file2] : lista di nomi di file da leggere dal server separati da ‘,’]\n\
[-R [n=0] : tale opzione permette di leggere ‘n’ file qualsiasi attualmente memorizzati nel server;\n\
			Se n=0 (o non è specificato) allora vengono letti tutti i file presenti nel server]\n\
[-d dirname : cartella in memoria secondaria dove scrivere i file letti dal server con l’opzione ‘-r’ o ‘-R’]\n\
[-t time : tempo in millisecondi che intercorre tra l’invio di due richieste successive al server]\n\
[-l file1[,file2] : lista di nomi di file su cui acquisire la mutua esclusione]\n\
[-u file1[,file2] : lista di nomi di file su cui rilasciare la mutua esclusione]\n\
[-c file1[,file2] : lista di file da rimuovere dal server se presenti]\n\
[-p : abilita le stampe sullo standard output per ogni operazione]\n"
					, argv[0]);
				exit(EXIT_SUCCESS);
			case 'f':
				conf->server_socket_filename = optarg;
				break;
			case 'w':
				conf->folder_to_write = mu_malloc(strlen(optarg) + 1);
				strcpy(conf->folder_to_write, optarg);
				conf->folder_to_write = strtok(conf->folder_to_write, ",");
				conf->folder_filecount = strtol(strtok(NULL, ""), NULL, 10);
				conf->folder_to_write = mu_realloc(conf->folder_to_write, strlen(conf->folder_to_write) + 1);
				break;
			case 'W':
				conf_add_list(optarg, &(conf->files_to_write));
				break;
			case 'D':
				conf->writeback_foldername = optarg;
				break;
			case 'r':
				conf_add_list(optarg, &(conf->files_to_read));
				break;
			case 'R':
				conf->read_filecount = strtol(optarg, NULL, 10);
				break;
			case 'd':
				conf->folder_destination = optarg;
				break;
			case 't':
				conf->connection_timer = strtol(optarg, NULL, 10);
				break;
			case 'l':
				conf_add_list(optarg, &(conf->files_to_lock));
				break;
			case 'u':
				conf_add_list(optarg, &(conf->files_to_unlock));
				break;
			case 'c':
				conf_add_list(optarg, &(conf->files_to_delete));
				break;
			case 'p':
				conf->verbose = 1;
				break;
			default: /* '?' */
				fprintf(stdout, "Usage: %s []", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
int printargs(client_conf *conf)
{
	printf(
"char *server_socket_filename = %s\n\
char *writeback_foldername = %s\n\
char *folder_to_write = %s\n\
char *folder_destination = %s\n\
unsigned int folder_filecount = %u\n\
unsigned int read_filecount = %u\n\
unsigned int connection_timer = %u\n\
bool verbose = %d\n", 
		conf->server_socket_filename != NULL ? conf->server_socket_filename : "NULL",
		conf->writeback_foldername != NULL ? conf->writeback_foldername : "NULL",
		conf->folder_to_write != NULL ? conf->folder_to_write : "NULL",
		conf->folder_destination != NULL ? conf->folder_destination : "NULL",
		conf->folder_filecount,
		conf->read_filecount,
		conf->connection_timer,
		conf->verbose
	);

	lu_print(conf->files_to_write, lu_string_print);
    lu_print(conf->files_to_read, lu_string_print);
    lu_print(conf->files_to_lock, lu_string_print);
    lu_print(conf->files_to_unlock, lu_string_print);
    lu_print(conf->files_to_delete, lu_string_print);

	return fflush(stdout);
}
int client_conf_cleanup(client_conf *conf)
{
	free(conf->folder_to_write);
	lu_free(&(conf->files_to_write), mu_free);
	lu_free(&(conf->files_to_read), mu_free);
	lu_free(&(conf->files_to_lock), mu_free);
	lu_free(&(conf->files_to_unlock), mu_free);
	lu_free(&(conf->files_to_delete), mu_free);
	return 0;
}
int conf_add_list(const char *optarg, u_list *list)
{
	char opt[strlen(optarg) + 1], *temp;
	strcpy(opt, optarg);
	temp = strtok(opt, ",");
	while(temp != NULL)
	{
        lu_insert_oncopy(list, temp, (strlen(temp) + 1), lu_string_compare);
		temp = strtok(NULL, ",");
	}

	return 0;
}
int write_nfiles_from_dir(const char *dirname, int nfiles, const char *wbdir)
{
	if (dirname == NULL)
	{
		fprintf(stderr, "write_nfiles_from_dir : param dirname == NULL\n");
		fflush(stderr);
		return -1;
	}
	if (nfiles == 0)
	{
		fprintf(stderr, "write_nfiles_from_dir : param nfiles == 0 -> nothing to do\n");
		fflush(stderr);
		return 0;
	}
	
	u_list filelist = NULL;
	u_list dirlist = NULL;
	u_list_node *currnode;
	char *currdir;
	int nfilesfound = 0;

	lu_insert_oncopy(&dirlist, (void *)dirname, strlen(dirname) + 1, lu_string_compare);
	
	while (lu_length(dirlist) != 0 && nfiles > 0)
	{
		currdir = lu_get_byindex(dirlist, 0);
		if ((nfilesfound = du_getfilepaths_from_dir(currdir, nfiles, &filelist, &dirlist)) == -1)
		{
			fprintf(stderr, "write_nfiles_from_dir : du_getfilepaths_from_dir returned an error\n");
			fflush(stderr);
			lu_free(&filelist, mu_free);
			lu_free(&dirlist, mu_free);
			return -1;
		}
		nfiles -= nfilesfound;
		lu_foreach(filelist, currnode, 
			printf("file %s\n", (char *)currnode->data);
		);
		write_files_list(filelist, wbdir);
		lu_free(&filelist, mu_free);
		lu_remove(&dirlist, currdir, lu_string_compare, mu_free);
	}
	lu_free(&dirlist, mu_free);
	return 0;
}
int write_files_list(u_list filelist, const char *wbdir)
{
	int res = 0;
	u_list_node *currnode;
	lu_foreach(filelist, currnode, 
		res += writeFile(currnode->data, wbdir);
	);
	return res ? -1 : 0;
}
int read_files_list(u_list filelist, const char *destdir)
{
	int res = 0;
	u_list_node *currnode;
	void *buff;
	size_t size;
	u_string temp = su_string_from_literal("");
	lu_foreach(filelist, currnode, 
		if (readFile((char *)currnode->data, &buff, &size))
		{
			fprintf(stderr, "\n");
			fflush(stderr);
			res++;
		} else {
			su_append_chars(&temp, destdir);
			su_append_chars(&temp, currnode->data);
			res += fu_writepath(temp.data, buff, size);
		}
	);
	return res ? -1 : 0;
}
int lock_files_list(u_list filelist)
{
	int res = 0;
	u_list_node *currnode;
	lu_foreach(filelist, currnode, 
		res += lockFile((char *)currnode->data);
	);
	return res ? -1 : 0;
}
int unlock_files_list(u_list filelist)
{
	int res = 0;
	u_list_node *currnode;
	lu_foreach(filelist, currnode, 
		res += unlockFile((char *)currnode->data);
	);
	return res ? -1 : 0;
}
int remove_files_list(u_list filelist)
{
	int res = 0;
	u_list_node *currnode;
	lu_foreach(filelist, currnode, 
		res += removeFile((char *)currnode->data);
	);
	return res ? -1 : 0;
}