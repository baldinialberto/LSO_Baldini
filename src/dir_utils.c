#include <dir_utils.h>
#include <string_utils.h>
#include <list_utils.h>
#include <mem_utils.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int du_getfilepaths_from_dir(const char *dirpath, int nfilepaths, u_list *filelist, u_list *dirlist)
{
    if (dirpath == NULL)
    {
        fprintf(stdout, "du_getfilepaths_from_dir : param dirpath == NULL\n");
        fflush(stdout);
        return -1;
    }
    if (nfilepaths == 0)
    {
        fprintf(stdout, "du_getfilepaths_from_dir : param nfilepaths == 0 -> nothing to do\n");
        fflush(stdout);
        return 0;
    }
    if (filelist == NULL)
    {
        fprintf(stdout, "du_getfilepaths_from_dir : param savelist == NULL\n");
        fflush(stdout);
        return -1;
    }

    int res = nfilepaths;

    DIR *dir = NULL;
    struct dirent *file;
    struct stat filestat;
    char filetype;
    u_string tempstring = su_string_from_literal("");

    if (chdir(dirpath) == -1)
    {
        perror("du_getfilepaths_from_dir : chdir");
        fprintf(stdout, "du_getfilepaths_from_dir : chdir returned an error\n");
        fflush(stdout);
        return -1;
    }
    if ((dir = opendir(".")) == NULL)
    {
        perror("du_getfilepaths_from_dir : opendir");
        fprintf(stdout, "du_getfilepaths_from_dir : opendir returned an error\n");
        fflush(stdout);
        return -1;
    }

    while (nfilepaths && (file = readdir(dir)) != NULL)
    {
        stat(file->d_name, &filestat);
		filetype = du_filenodetype(&filestat);

		if (filetype == 'd' && strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0)
		{
            su_append_chars(&tempstring, dirpath);
            su_append_chars(&tempstring, "/");
            su_append_chars(&tempstring, file->d_name);
            lu_insert(dirlist, mu_clone(tempstring.data, tempstring.len + 1));
            su_realloc(&tempstring, 0);
		}
		else if (filetype == '-')
		{
            su_append_chars(&tempstring, dirpath);
            su_append_chars(&tempstring, "/");
            su_append_chars(&tempstring, file->d_name);
			if (lu_insert(filelist, mu_clone(tempstring.data, tempstring.len + 1)) != -1)
                nfilepaths--;
            su_realloc(&tempstring, 0);
		}
    }
    su_free_string(&tempstring);

    if (closedir(dir) == -1)
    {
        perror("du_getfilepaths_from_dir : closedir");
        fprintf(stdout, "du_getfilepaths_from_dir : closedir returned an error\n");
        fflush(stdout);
        return -1;
    }

    return res - nfilepaths;
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