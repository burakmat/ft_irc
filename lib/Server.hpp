#pragma once
#include "User.hpp"

class Server : public Socket
{
	private:
		std::string host_name;
		std::string created_time;
	public:
		std::vector<struct pollfd> pfds;
		std::vector<User> user_list;
	
		Server(int port);
		~Server();

		void acception();
		
		void delete_fd(int index);
		void create_fd(int fd);
		
		void delete_user();
		void create_user(std::string user_name, std::string nick_name, std::string real_name);

		void getting_command(int index, std::string msg);
		void create_msg(int index, std::string code, std::string msg);
		void send_msg(int fd, std::string msg);
	
		std::string get_host_name();

};

