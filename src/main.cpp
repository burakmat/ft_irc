#include "Server.hpp"

int main(int ac, char **av)
{
	(void)ac;
	(void)av;

	struct pollfd mypoll;
	int counter = 0;
	char buffer[256];

	memset(&mypoll, 0, sizeof(mypoll));
	mypoll.events = POLL_IN;

	Server server(atoi(av[1]));

	// This part is still blocking. Needs to change.
	std::cout << "Waiting for a client to connect. (accept)" << std::endl;
	server.acception();


	mypoll.fd = server.get_client_fd();
	std::cout << "A client is connected!" << std::endl;

	while (1)
	{
		std::cout << "Listening on port " << av[1] << "." << std::endl;
		if (poll(&mypoll, 1, 1000) == 1) {
			read(mypoll.fd, buffer, sizeof(buffer));
			std::cout << "Client: " << buffer << std::endl;
			break;
		}
		else
			counter += 1;
	}

	std::cout << counter * 1000 << " ms passed." << std::endl;

}