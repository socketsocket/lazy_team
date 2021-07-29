#include "Webserv.hpp"

void	initStatusCodeMap() {
	for (size_t i = 0; i < sizeof(status_code_arr); ++i)
		status_code_map[std::string(status_code_arr[i]).substr(0, 3)]
			= status_code_arr[i];
}

int	main() {
	return (OK);
}