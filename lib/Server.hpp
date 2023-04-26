#pragma once
#include "Socket.hpp"

class Server : public Socket
{
	private:
		int last_client_fd;
	public:
		struct pollfd pfds[MAX_CLIENT + 1];
	
		Server(int port);
		~Server();

		void acception();
		int get_client_fd() const;
};

