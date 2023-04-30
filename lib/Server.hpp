#pragma once
#include "User.hpp"

class Server : public Socket
{
	private:
	public:
		std::vector<struct pollfd> pfds;
		// std::vector<User> user_list;
	
		Server(int port);
		~Server();

		void acception();
		
		void delete_fd(int index);
		void create_fd(int fd);
		
		void delete_user();
		void create_user();

};
