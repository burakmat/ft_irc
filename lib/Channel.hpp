#pragma once
#include "Socket.hpp"
#include "User.hpp"

class Channel
{
	private:
		std::string name;
		std::string topic;
		std::string password;
		std::vector<std::string> mode_list;
		std::string creator;
		std::vector<std::string> invited_list;

	public:
		std::vector<User> user_list;
		std::vector<std::string> mod_users;

		Channel(std::string name);
		~Channel();

		bool add_to_channel(User user);
		bool remove_from_channel(User user);
		bool user_exists(User user) const;
		bool user_exists_name(std::string name) const;
		
		std::string get_name() const;
		void set_topic(std::string str);
		std::string get_topic() const;
		std::string get_str_user_list() const;
		std::vector<User> get_user_list() const;
		std::string get_password() const;
		void set_password(std::string password);
		int verified_size() const;
		void add_mode(std::string mode);
		bool is_exist_mode(std::string mode);
		void remove_mode(std::string mode);
		void send_message(User sender, std::string message, bool to_all) const;
		
		void set_creator(std::string creator);
		std::string get_creator() const;

		bool is_mode_users(std::string name);
		void add_mode_users(std::string name);
		void remove_mode_users(std::string name);
		void msg_for_mode_users(int fd, std::string msg);

		// Invite utils
		void set_inv_only(bool inv_only);
		bool is_inv_only();

		bool add_invite(std::string nick);
		bool remove_invite(std::string nick);
		bool is_invited(std::string nick) const;

		//debug 
		void print_user_list() const;
};
