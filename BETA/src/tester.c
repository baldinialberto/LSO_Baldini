#include "../include/file_utils.h"

int main(void)
{
    u_file_storage fstorage = fu_init_file_storage(100, 0xA << 20);
    u_file *file = fu_allocfile_empty("test.sh");
    fu_fread("test.sh", file->data);
    fu_fwrite(file->data, "test3.sh");
    fu_filefree(file);
    fu_storage_free(&fstorage);
    return 0;
}