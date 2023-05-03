#include "Server.hpp"

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "./server <port> <password>" << std::endl;
		return 1;
	}
	
	size_t n_byte;

	Server server(atoi(av[1]), av[2]);

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
					if (server.pfds.size() - 1 == server.user_list.size()) {
						server.remove_from_all_channels(server.user_list[i - 1], i);
						server.delete_user(i);
					}
					server.delete_fd(i);
				} else {
					std::cout  << "--------------------" << std::endl << "Client " << i << ": " << buffer << std::endl;
					server.getting_command(i, buffer);
				}

				server.pfds[i].revents = 0;
			}
		}
	}
}
