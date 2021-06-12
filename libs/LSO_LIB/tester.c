//
// Created by Alberto Baldini on 04/06/21.
//

#include "file_utils.h"
#include "mem_utils.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    u_file_storage storage = fu_init_file_storage(1000, 1 << 20);

    char temp_string[256] = {0};
    void *temp_data = NULL;
    size_t temp_len = 0;

    for (int i = 0; i < 100; i++)
    {
        sprintf(temp_string, "../../to_write/file_%d.txt", i);
        fu_read_from_path(temp_string, &temp_data, &temp_len);
        fu_add_file(&storage, fu_alloc_file_data(mu_clone(temp_data, temp_len), temp_len),
                    mu_clone(temp_string, strlen(temp_string) + 1));
        free(temp_data);
        temp_data = NULL;
        temp_len = 0;
    }

    fu_storage_print(&storage);
    fu_storage_free(&storage);

    return 0;
}
