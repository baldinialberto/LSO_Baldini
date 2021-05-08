#include <server_utils.h>

int main(int argc, char** argv)
{
	server_settings settings = parse_settings();
	print_server_settings(&settings);
	write_log("Tigre Sul Puff", &settings);
	return EXIT_SUCCESS;
}
