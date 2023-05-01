#include "Channel.hpp"

Channel::Channel(std::string name)
: name(name)
{
}

Channel::~Channel()
{
}

// Returns false if given fd is already in the channel
bool Channel::add_to_channel(int fd)
{
	if (std::find(fds.begin(), fds.end(), fd) != fds.end()) {
		fds.push_back(fd);
		return true;
	}
	return false;
}

// Returns false if given fd is not in the channel
bool Channel::remove_from_channel(int fd)
{
	std::vector<int>::iterator it = std::find(fds.begin(), fds.end(), fd);
	if (it == fds.end())
		return false;
	fds.erase(it);
	return true;
}

std::string Channel::get_channel_name() const
{
	return name;
}

// Does server send the message to sender???
void Channel::send_message(int sender_fd, std::string message) const
{
	for (std::vector<int>::const_iterator it = fds.begin(); it != fds.end(); ++it) {
		if (*it != sender_fd) {
			write(*it, message.c_str(), message.length());
		}
	}
}