#pragma once
#include "User.hpp"
#include "Channel.hpp"

class Server : public Socket
{
	private:
		std::string host_name;
		std::string created_time;
		std::string password;

		int is_nickname_exist(std::string nick) const;
		int is_channel_active(std::string);
		void command_user(int index, std::vector<std::string> array);
		void command_privmsg(int index, std::vector<std::string> array);
		void command_join(int index, std::vector<std::string> array);
		void command_topic(int index, std::vector<std::string> array);
		void command_who(int index, std::vector<std::string> array);
		void command_part(int index, std::vector<std::string> array);
		void command_quit(int index);

	public:
		std::vector<struct pollfd> pfds;
		std::vector<User> user_list;
		std::vector<Channel> channel_list;
	
		Server(int port, std::string _password);
		~Server();

		void acception();
		void getting_command(int index, std::string msg);
		
		std::vector<std::string> parse(std::string input);
		std::string create_msg(int index, std::string code, std::string msg);
		std::string create_msg_2(int index, std::string msg);
		void user_to_user(int index, std::string command, std::string receiver_nick_name, std::string msg);
		void send_msg(int fd, std::string msg);
		int verified_size() const;
	
		//CHANNEL
		Channel &find_channel(std::string name);
		void remove_from_all_channels(User user, int index);

		// USER/FD CREATE/DELETE
		void delete_fd(int index);
		void create_fd(int fd);
		
		void delete_user(int index);

		//GETTERS and SETTERS
		std::string get_host_name();
		void set_time();
		void set_host_name();
		void set_user(std::string user_name, std::string nick_name, std::string real_name, int index);
		
		// DEBUG
		void print_info() const;
};

