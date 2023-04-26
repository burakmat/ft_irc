#include "Server.hpp"

int main(int ac, char **av)
{
	(void)ac;
	(void)av;

	size_t n_byte;

	Server server(atoi(av[1]));

	int nfds = 0;

	std::cout << "Waiting for a client to connect. (accept)" << std::endl;


	while (1)
	{
		// Third parameter (timeout) can be changed.
		if (poll(server.pfds, MAX_CLIENT + 1, 1000) == -1)
			exit(1);
		std::cout << "Listening on port " << av[1] << "." << std::endl;
		if (server.pfds[0].revents & POLLIN) {
			if (server.fail())
				return 1;
			server.acception();
			if (nfds < MAX_CLIENT) {
				server.pfds[nfds + 1].fd = server.get_client_fd();
				server.pfds[nfds + 1].events = POLLIN;
				++nfds;
			} else {
				std::cout << "Too many clients!" << std::endl;
				close(server.get_client_fd());
			}
		}
		for (int i = 1; i < MAX_CLIENT + 1; ++i) {
			if (server.pfds[i].revents & POLLIN) {
				char buffer[256];
				n_byte = read(server.pfds[i].fd, buffer, sizeof(buffer));
				if (n_byte == 0) {
					close(server.pfds[i].fd);
					server.pfds[i].fd = 0;
					server.pfds[i].events = 0;
					--nfds;
				} else {
					std::cout << "Client " << i << ": " << buffer << std::endl;
				}
				server.pfds[i].revents = 0;
			}
		}
	}


}