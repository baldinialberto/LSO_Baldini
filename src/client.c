#include <client.h>

//static int socket_fd;

void test()
{
	socket_fd = 0;
	struct timespec connectionTime;
	connectionTime.tv_sec = 0;
	connectionTime.tv_nsec = 999999999;

	if (openConnection("MyServerSocket.socket", 200, connectionTime))
	{
		printf("Unable to connect to server\n");
		exit(1);
	}
	else 
		printf("Connected\n");


	printf("Connected to %d\n", socket_fd);
	//openFile("Pippo.txt", O_CREATE_FLAG | O_LOCK_FLAG);
	//unlockFile("Pippo.txt");
	//closeFile("Pippo.txt");

	sleep(20);

	if (closeConnection("MyServerSocket.socket"))
		printf("Unable to disconnect\n");
	else
		printf("disconnected\n");
}

int main(int argc, char** argv)
{
	assert(sizeof(server_command_t) == 4);
	
	client_conf conf = {0};
	parseargs(argc, argv, &conf);
	//printargs(&conf);

	//test();

	client_conf_cleanup(&conf);	

	return 0;
}

int parseargs(int argc, char ** argv, client_conf *conf)
{
	int opt;
	const char *flags = "hf:w:W:D:r:R:d:t:l:u:c:p";

	while ((opt = getopt(argc, argv, flags)) != -1)
	{
		printf("optind = %d, opt = %c, optarg = %s, opterr = %d, optopt = %d\n", 
			optind, opt, optarg, opterr, optopt
		);
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
				CHECK_BADVAL_PERROR_EXIT(
					conf->folder_to_write = malloc(strlen(optarg) + 1), 
					NULL, "parseargs : malloc"
				);
				strcpy(conf->folder_to_write, optarg);
				conf->folder_to_write = strtok(conf->folder_to_write, ",");
				conf->folder_filecount = strtol(strtok(NULL, ""), NULL, 10);
				CHECK_BADVAL_PERROR_EXIT(
					conf->folder_to_write = realloc(conf->folder_to_write, 
					strlen(conf->folder_to_write) + 1), 
					NULL, "parseargs : realloc"
				);
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
	ln_ptr temp;
	printf("files_to_write\n");
	LIST_FOREACH(conf->files_to_write, temp, 
		printf("%s->", (char*)temp->data)
	);
	printf("\nfiles_to_read\n");
	LIST_FOREACH(conf->files_to_read, temp, 
		printf("%s->", (char*)temp->data)
	);
	printf("\nfiles_to_lock\n");
	LIST_FOREACH(conf->files_to_lock, temp, 
		printf("%s->", (char*)temp->data)
	);
	printf("\nfiles_to_unlock\n");
	LIST_FOREACH(conf->files_to_unlock, temp, 
		printf("%s->", (char*)temp->data)
	);
	printf("\nfiles_to_delete\n");
	LIST_FOREACH(conf->files_to_delete, temp, 
		printf("%s->", (char*)temp->data)
	);
	printf("\n");

	return fflush(stdout);
}

int client_conf_cleanup(client_conf *conf)
{
	free(conf->folder_to_write);
	list_free(&(conf->files_to_write));
	list_free(&(conf->files_to_read));
	list_free(&(conf->files_to_lock));
	list_free(&(conf->files_to_unlock));
	list_free(&(conf->files_to_delete));
	return 0;
}

int conf_add_list(const char *optarg, ln_ptr* list)
{
	printf("conf_add_list [%s]\n", optarg);
	char opt[strlen(optarg) + 1], *temp, *temp2 = NULL;

	strcpy(opt, optarg);
	temp = strtok(opt, ",");
	while(temp != NULL)
	{
		printf("temp = [%s]\n", temp);
		CHECK_BADVAL_PERROR_EXIT(
			temp2 = malloc((strlen(temp) + 1) * sizeof(char)), 
			NULL, "parseArgs : malloc"
		);
		strcpy(temp2, temp);
		list_insert(list, list_allocnode((void *)temp2), string_compare);
		temp = strtok(NULL, ",");
	}

	return 0;
}