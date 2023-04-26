#include "Server.hpp"

Server::Server(int port) : Socket::Socket(port)
{
	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
		_fail = true;
    }
	if (listen(fd, MAX_CLIENT) < 0)
    {
        perror("In listen");
		_fail = true;
    }
	for (int i = 1; i < MAX_CLIENT + 1; ++i) {
		pfds[i].fd = 0;
		pfds[i].events = 0;
		pfds[i].revents = 0;
	}
	pfds[0].fd = fd;
	pfds[0].events = POLLIN;
	pfds[0].revents = 0;

}

Server::~Server()
{

}

void Server::acception()
{
	last_client_fd = accept(fd, 0, 0);
	if (last_client_fd < 0)
	{
		perror("In accept");
		_fail = true;
	}
}

int Server::get_client_fd() const
{
	return last_client_fd;
}
