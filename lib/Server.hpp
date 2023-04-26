#pragma once
#include "Socket.hpp"

class Server : public Socket
{
	private:
		int client_fd;
	public:
		Server(int port);
		~Server();

		void acception();
		int get_client_fd() const;
};

