#include "Socket.hpp"

Socket::Socket() : _fail(false)
{
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = INADDR_ANY;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		_fail = true;
	}
}

Socket::Socket(int port) : _fail(false)
{
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		_fail = true;
	}
}

Socket::~Socket()
{
}

bool Socket::fail() const
{
	return _fail;
}

int Socket::get_fd() const
{
	return fd;
}
