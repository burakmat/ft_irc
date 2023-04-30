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
		std::cout << "Listening on port " << av[1] << "." << std::endl;


		if (server.pfds[0].revents & POLLIN) {
			server.acception();
		}

		for (int i = 1; i < (int)server.pfds.size(); ++i) {

			if (server.pfds[i].revents & POLLIN) {
				char buffer[256];
				n_byte = read(server.pfds[i].fd, buffer, sizeof(buffer));
				server.create_user();
				if (n_byte == 0) {
					std::cout << "lala " << std::endl;
					server.delete_fd(i);
					server.delete_user();
				} else {
					std::cout << "Client " << i << ": " << buffer << std::endl;
				}

				server.pfds[i].revents = 0;
				memset(buffer, 0, sizeof(buffer));
			}
		}
	}
}
// std::string msg=":osman 001 osman23 :Hi, welcome to IRC\r\n:osman 002 osman23 :Your host is osman, running version miniircd-2.2\r\n:osman 003 osman23 :This server was created sometime\r\n:osman 004 osman23 osman miniircd-2.2 o o\r\n:osman 251 osman23 :There are 1 users and 0 services on 1 server\r\n:osman 422 osman23 :MOTD File is missing\r\n";