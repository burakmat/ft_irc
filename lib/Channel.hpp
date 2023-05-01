#pragma once
#include "Socket.hpp"

class Channel
{
	private:
		std::string name;
		std::vector<int> fds;
	public:
		Channel(std::string name);
		~Channel();

		bool add_to_channel(int fd);
		bool remove_from_channel(int fd);
		std::string get_channel_name() const;
		void send_message(int sender_fd, std::string message) const;
};
