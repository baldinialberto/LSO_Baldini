#include "../include/file_utils.h"

int main(int argc, char **argv)
{
    u_file_storage fstorage = fu_init_file_storage(100, 0xA << 20);
    char temp[256] = {(char)0};
    int n = (argc == 2) ? strtol(argv[1], NULL, 10) : 10;
    u_file file;
    for (int i = 0; i < n; i++)
    {
        printf("---%d---\n", i);
        sprintf(temp, "/home/alberto/LSO_Baldini/towrite/meta_%d.txt", i);
        file = fu_allocfile_empty(temp);
        fu_fread(file.path, file.data);
        fu_addfile(&fstorage, file.data, file.path);
        sprintf(temp, "/home/alberto/LSO_Baldini/readen/meta_%d.txt", i);
        fu_fwrite(fu_getfile(&fstorage, file.path), temp);
    }
    // u_file file = fu_allocfile_empty("test.sh");
    // fu_fread("test.sh", file.data);
    // fu_addfile(&fstorage, file.data, file.path);
    // fu_fwrite(fu_getfile(&fstorage, "test.sh"), "test4.sh");
    fu_storage_free(&fstorage);
    return 0;
}