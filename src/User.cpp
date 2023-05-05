#include "User.hpp"


User::User(int _fd)
: fd(_fd),
verified(false),
user_name(""),
nick_name(""),
real_name("")
{}

User::~User() {
}

std::string User::get_user_name() const {
	return this->user_name;
}

void User::set_user_name(std::string _user_name) {
	user_name = _user_name;
}

void User::set_nick_name(std::string _nick_name) {
	nick_name = _nick_name;
}

void User::set_real_name(std::string _real_name) {
	real_name = _real_name;
}

std::string User::get_nick_name() const {
	return this->nick_name;
}

std::string User::get_real_name() const {
	return this->real_name;
}

// Returns true if user verified.
bool User::is_verified() const
{
	return verified;
}

void User::set_verified(bool truth)
{
	verified = truth;
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