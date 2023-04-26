#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>

#define PORT 8080


class Socket
{
	protected:
		struct sockaddr_in address;
		int fd;
		bool _fail;
	public:
		Socket();
		Socket(int port);
		~Socket();

		bool fail() const;
		int get_fd() const;
};

