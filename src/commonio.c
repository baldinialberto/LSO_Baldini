#include <commonio.h>

int f_skipline(FILE *fstream)
{
    int res = 0, res2 = 0;

    CHECK_BADVAL_PERROR_RETURN(
        (res = fscanf(fstream, "%*[^\n]")), 
        -1, "fscanf", -1
    );
    CHECK_BADVAL_PERROR_RETURN(
        (res2 = fscanf(fstream, "%*c")), 
        -1, "fscanf", -1
    );

    return res + res2;
}

int f_readline(FILE *fstream, char **str)
{
    int lineLen = 0;
    
    CHECK_BADVAL_PERROR_RETURN(
        (lineLen = fscanf(fstream, "%*[^\n]")), 
        -1, "fscanf", -1
    );
    CHECK_BADVAL_PERROR_RETURN(
        (0 != fseek(fstream, -lineLen, SEEK_CUR)), 
        1, "fseek", -1
    );
    CHECK_BADVAL_PERROR_EXIT(
        (*str = (char *)malloc((lineLen + 1) * sizeof(char))),
        NULL, "malloc"
    );
    CHECK_COND_RETURN(
        fread((void *)*str, sizeof(char), lineLen, fstream) < lineLen, 
        -1, "fread"
    );
    return 0;
}
    
int f_readword(FILE *fstream, char **str)
{
    int wordLen = 0;
    
    CHECK_BADVAL_PERROR_RETURN(
        (wordLen = fscanf(fstream, "%*[^ ,=\r\n]")), 
        -1, "fscanf", -1
    );
    CHECK_BADVAL_PERROR_RETURN(
        (0 != fseek(fstream, -wordLen, SEEK_CUR)), 
        1, "fseek", -1
    );
    CHECK_BADVAL_PERROR_EXIT(
        (*str = (char *)malloc((wordLen + 1) * sizeof(char))),
        NULL, "malloc"
    );
    CHECK_COND_RETURN(
        fread((void *)*str, sizeof(char), wordLen, fstream) < wordLen, 
        -1, "fread"
    );
    (*str)[wordLen] = (char) 0;

    return 0;
}