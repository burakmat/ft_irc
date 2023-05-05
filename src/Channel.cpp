#include "Channel.hpp"

Channel::Channel(std::string name)
: name(name), topic("No topic is set"), password("")
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
			if (creator == (*it).get_nick_name() && user_list.size() > 1){
				set_creator(user_list[1].get_nick_name());
				add_mode_users(user_list[1].get_nick_name());
			}
			
			remove_mode_users((*it).get_nick_name());
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

// Sends the given message to the every fd except given user's (if to_all is false).
void Channel::send_message(User sender, std::string message, bool to_all) const
{
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if ((*it).get_fd() != sender.get_fd() || to_all) {
			std::cout << "Sending back : " << message << std::endl;
			write((*it).get_fd(), message.c_str(), message.length());
		}
	}
}

void Channel::add_mode(std::string mode)
{
	if (!is_exist_mode(mode))
		mode_list.push_back(mode);
}

//return true if mode exists
bool Channel::is_exist_mode(std::string mode)
{
	for (std::vector<std::string>::iterator it = mode_list.begin(); it != mode_list.end(); ++it) {
		if ((*it) == mode) {
			return true;
		}
	}
	return false;
}

void Channel::remove_mode(std::string mode)
{
	for (std::vector<std::string>::iterator it = mode_list.begin(); it != mode_list.end(); ++it) {
		if ((*it) == mode) {
			mode_list.erase(it);
			return;
		}
	}
}

std::string Channel::get_password() const
{
	return password;
}

void Channel::set_password(std::string password)
{
	this->password = password;
}

void Channel::set_creator(std::string creator)
{
	this->creator = creator;
}

std::string Channel::get_creator() const
{
	return creator;
}

bool Channel::is_mode_users(std::string name)
{
	for (std::vector<std::string>::const_iterator it = mod_users.begin(); it != mod_users.end(); ++it) {
		if (((*it) == name)) {
			return true;
		}
	}
	return false;
}

void Channel::add_mode_users(std::string name)
{
	if (!is_mode_users(name)){
		mod_users.push_back(name);
	}
}

void Channel::remove_mode_users(std::string name)
{
	for (std::vector<std::string>::iterator it = mod_users.begin(); it != mod_users.end(); ++it) {
		if ((*it) == name) {
			mod_users.erase(it);
			return;
		}
	}
}

bool Channel::user_exists_name(std::string name) const 
{
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if ((*it).get_nick_name() == name) {
			return true;
		}
	}
	return false;
}


//DEBUG

void Channel::print_user_list() const
{
	for (std::vector<std::string>::const_iterator it = mod_users.begin(); it != mod_users.end(); ++it) {
		std::cout << "operator users: " << (*it) << std::endl;
	}
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		std::cout << "normal users: " << (*it).get_nick_name() << std::endl;
	}

}