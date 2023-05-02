#include "Channel.hpp"

Channel::Channel(std::string name)
: name(name)
{
}

Channel::~Channel()
{
}

// Returns false if given user is already in the channel
bool Channel::add_to_channel(User user)
{
	for (std::vector<User>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if ((*it).get_nick_name() == user.get_nick_name()) {
			return false;
		}
	}
	user_list.push_back(user);
	return true;

}

// Returns false if given user is not in the channel
bool Channel::remove_from_channel(User user)
{
	for (std::vector<User>::iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if (user.get_nick_name() == (*it).get_nick_name()) {
			user_list.erase(it);
			return true;
		}
	}
	return false;
}

std::string Channel::get_channel_name() const
{
	return name;
}

void Channel::set_topic(std::string str)
{
	topic = str;
}

std::string Channel::get_topic() const {
	return topic;
}

std::string Channel::get_str_user_list() const {
	std::string users = "";
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		users += (*it).get_nick_name();
		if ((it + 1) != user_list.end()) {
			users += " ";
		}
	}
	return users;
}

// Does server send the message to sender???
void Channel::send_message(User sender, std::string message) const
{
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if ((*it).get_fd() != sender.get_fd()) {
			write((*it).get_fd(), message.c_str(), message.length());
		}
	}
}