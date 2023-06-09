#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>

#define MAX_CLIENT 10
#define USER_ID (index-1)

class Socket
{
	protected:
		struct sockaddr_in address;
		int fd;
	public:
		Socket(int port);
		~Socket();

		void fail_check(int fail);
		int get_fd() const;
};

