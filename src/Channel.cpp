#include "Channel.hpp"

Channel::Channel(std::string name)
: name(name), topic("No topic is set")
{
}

Channel::~Channel()
{
}

// Returns false if given user is already in the channel
bool Channel::add_to_channel(User user)
{
	if (!user_exists(user))
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
// Returns true if the given user exists on the channel, false otherwise.
bool Channel::user_exists(User user) const {
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if ((*it).get_nick_name() == user.get_nick_name()) {
			return true;
		}
	}
	return false;
}

std::string Channel::get_name() const
{
	return name;
}

std::vector<User> Channel::get_user_list() const
{
	return user_list;
}

int Channel::verified_size() const
{
	int counter = 0;
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if ((*it).is_verified()) {
			++counter;
		}
	}
	return counter;
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

// Sends the given message to the every fd except given user's.
void Channel::send_message(User sender, std::string message) const
{
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if ((*it).get_fd() != sender.get_fd()) {
			write((*it).get_fd(), message.c_str(), message.length());
		}
	}
}