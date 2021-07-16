#include <server.h>
#include <sys_utils.h>
#include <print_utils.h>
#include <common_utils.h>

int main(int argc, char **argv)
{
    DEBUG_(pu_print_flush(stdout, "%s\n", __func__));
    DEBUG_(pu_print_flush(stdout, "stdout\n"));
    DEBUG_(pu_print_flush(stderr, "stderr\n"));
    DEBUG_(pu_print_pos(stdout, "test"));

    static server_data s_data;



    return 0;
}
