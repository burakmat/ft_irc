#include "Server.hpp"

Server::Server(int port) : Socket::Socket(port)
{
	if (bind(fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
		_fail = true;
    }
	if (listen(fd, 1) < 0)
    {
        perror("In listen");
		_fail = true;
    }
}

Server::~Server()
{

}

void Server::acception()
{
	client_fd = accept(fd, 0, 0);
	if (client_fd < 0)
	{
		perror("In accept");
		_fail = true;
	}
}

int Server::get_client_fd() const
{
	return client_fd;
}
