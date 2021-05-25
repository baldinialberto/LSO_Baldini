#include "../include/file_utils.h"

int main(void)
{
    u_file_storage fstorage = fu_init_file_storage(100, 0xA << 20);

    fu_storage_free(&fstorage);
    return 0;
}