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
    int lineLen = 0, lineLen_post;

    size_t currPos = ftell(fstream);
    CHECK_ERRNO_RETURN(0, ftell);
    
    CHECK_BADVAL_PERROR_RETURN(
        (lineLen = fscanf(fstream, "%*[^\n]")), 
        -1, "fscanf", -1
    );
    CHECK_BADVAL_PERROR_RETURN(
        (lineLen != fseek(fstream, -lineLen, SEEK_CUR)), 
        1, "fseek", -1
    );
    CHECK_BADVAL_PERROR_EXIT(
        (*str = (char *)malloc((lineLen + 1) * sizeof(char))),
        NULL, "malloc"
    );
    //lineLen_post = fread
}
    
int f_readword(FILE *fstream, char **str)
{
    return 0;
}