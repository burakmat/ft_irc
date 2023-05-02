#pragma once
#include "User.hpp"
#include "Channel.hpp"

class Server : public Socket
{
	private:
		std::string host_name;
		std::string created_time;

		bool is_nickname_unique(std::string nick) const;
		int is_channel_active(std::string) const;
	public:
		std::vector<struct pollfd> pfds;
		std::vector<User> user_list;
		std::vector<Channel> channel_list;
	
		Server(int port);
		~Server();

		void acception();
		
		void delete_fd(int index);
		void create_fd(int fd);
		
		void delete_user(int index);
		void create_user(std::string user_name, std::string nick_name, std::string real_name, int fd);

		void getting_command(int index, std::string msg);
		void create_msg(int index, std::string code, std::string msg);
		void send_msg(int fd, std::string msg);
	
		std::string get_host_name();

		void join_channel(std::string channel, int index);
		void remove_from_all_channels(User user);
};

