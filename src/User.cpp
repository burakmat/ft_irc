#include "User.hpp"


User::User(int _fd)
: user_name(""),
nick_name(""),
real_name(""),
fd(_fd)
{}

User::User(std::string user_name, std::string nick_name, std::string real_name, int fd) {
	this->user_name = user_name;
	this->nick_name = nick_name;
	this->real_name = real_name;
	this->fd = fd;
}

User::~User() {
}

std::string User::get_user_name() const {
	return this->user_name;
}

std::string User::get_nick_name() const {
	return this->nick_name;
}

std::string User::get_real_name() const {
	return this->real_name;
}

void User::set_names(std::string _user_name, std::string _nick_name, std::string _real_name)
{
	user_name = _user_name;
	nick_name = _nick_name;
	real_name = _real_name;
}


int User::get_fd() const {
	return this->fd;
}