#include <commonio.h>

size_t f_skipline(FILE *fstream)
{
    size_t res = fscanf(fstream, "%*[^\n]");
	res += fscanf(fstream, "%*c");

    return res;
}

size_t f_readline(FILE *fstream, char **str)
{
    int lineLen = 0;
    CHECK_BADVAL_PERROR_RETURN(
        (lineLen = fscanf(fstream, "%*[^\n]")), 
        -1, "fscanf", -1
    );
    CHECK_BADVAL_PERROR_RETURN(
        (lineLen != fseek(fstream, -lineLen, SEEK_CUR)), 
        1, "fseek", -1
    )
    CHECK_BADVAL_PERROR_EXIT(
        (*str = (char *)malloc((lineLen + 1) * sizeof(char))),
        NULL, "malloc"
    )
}
    
size_t f_readword(FILE *fstream, char **str)
{
    return 0;
}