#include "Socket.hpp"

Socket::Socket(int port)
{
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = INADDR_ANY;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("In socket");
		exit(1);
	}
}

Socket::~Socket()
{
}

void Socket::fail_check(int fail){
	if (fail < 0)
	{
		std::cout << "ERROR" << std::endl;
		exit(1);
 	}
}

int Socket::get_fd() const
{
	return fd;
}
