#include "../include/dir_utils.h"

int du_getfilepaths_from_dir(const char *dirpath, int nfilepaths, int maxDepth, u_list *savelist)
{
    if (dirpath == NULL)
    {
        fprintf(stderr, "du_getfilepaths_from_dir : param dirpath == NULL\n");
        fflush(stderr);
        return -1;
    }
    if (nfilepaths == 0)
    {
        fprintf(stderr, "du_getfilepaths_from_dir : param nfilepaths == 0 -> nothing to do\n");
        fflush(stderr);
        return 0;
    }
    if (savelist == NULL)
    {
        fprintf(stderr, "du_getfilepaths_from_dir : param savelist == NULL\n");
        fflush(stderr);
        return -1;
    }

    DIR *dir = NULL;
    struct dirent *file;
    struct stat filestat;
    char filetype;
    u_string tempstring = su_string_from_literal("");

    if (chdir(dirpath) == -1)
    {
        perror("du_getfilepaths_from_dir : chdir");
        fprintf(stderr, "du_getfilepaths_from_dir : chdir returned an error\n");
        fflush(stderr);
        return -1;
    }
    if ((dir = opendir(".")) == NULL)
    {
        perror("du_getfilepaths_from_dir : opendir");
        fprintf(stderr, "du_getfilepaths_from_dir : opendir returned an error\n");
        fflush(stderr);
        return -1;
    }

    // discard dir . and ..
    if (readdir(dir) == NULL)
    {
        fprintf(stderr, "du_getfilepaths_from_dir : readdir returned NULL\n");
        fflush(stderr);
        return -1;
    }
    if (readdir(dir) == NULL)
    {
        fprintf(stderr, "du_getfilepaths_from_dir : readdir returned NULL\n");
        fflush(stderr);
        return -1;
    }

    while (nfilepaths && (file = readdir(dir)) != NULL)
    {
        stat(file->d_name, &filestat);
		filetype = du_filenodetype(&filestat);

		if (filetype == 'd')
		{
            printf("found dir = %s\n", file->d_name);
		}
		else if (filetype == '-')
		{
			lu_insert_oncopy(savelist, file->d_name, strlen(file->d_name) + 1, lu_string_compare);
            nfilepaths--;
        }
    }

    su_free_string(&tempstring);

    if (closedir(dir) == -1)
    {
        perror("du_getfilepaths_from_dir : closedir");
        fprintf(stderr, "du_getfilepaths_from_dir : closedir returned an error\n");
        fflush(stderr);
        return -1;
    }

    return 0;
}

char du_filenodetype(struct stat *_stat)
{
	if (S_ISDIR(_stat->st_mode))
	{
		return 'd';
	}
	else if (S_ISSOCK(_stat->st_mode))
	{
		return 's';
	}
	else if (S_ISLNK(_stat->st_mode))
	{
		return 'l';
	}
	else
	{
		return '-';
	}
}