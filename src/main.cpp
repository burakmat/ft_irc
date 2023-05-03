#include "Server.hpp"

int main(int ac, char **av)
{
	if (ac < 2)
	{
		std::cout << "./server <port>" << std::endl;
		return 1;
	}
	
	size_t n_byte;

	Server server(atoi(av[1]));


	std::cout << "Waiting for a client to connect. (accept)" << std::endl;


	while (1)
	{
		// Third parameter (timeout) can be changed.

		if (poll(&server.pfds[0], server.pfds.size(), 1000) == -1)
			exit(1);

		if (server.pfds[0].revents & POLLIN) {
			server.acception();
		}

		for (int i = 1; i < (int)server.pfds.size(); ++i) {

			if (server.pfds[i].revents & POLLIN) {
				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				n_byte = read(server.pfds[i].fd, buffer, sizeof(buffer));
				
				if (n_byte == 0) {
					server.remove_from_all_channels(server.user_list[i - 1], i);
					server.delete_fd(i);
					server.delete_user(i);
				} else {
					std::cout  << "--------------------" << std::endl << "Client " << i << ": " << buffer << std::endl;
					server.getting_command(i, buffer);
				}

				server.pfds[i].revents = 0;
			}
		}
	}
}
