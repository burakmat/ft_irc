#pragma once
#include "Socket.hpp"
#include "User.hpp"

class Channel
{
	private:
		std::string name;
		std::vector<User> user_list;
		std::string topic;
	public:
		Channel(std::string name);
		~Channel();

		bool add_to_channel(User user);
		bool remove_from_channel(User user);
		bool user_exists(User user) const;
		
		std::string get_name() const;
		void set_topic(std::string str);
		std::string get_topic() const;
		std::string get_str_user_list() const;
		std::vector<User> get_user_list() const;

		void send_message(User sender, std::string message) const;
};
