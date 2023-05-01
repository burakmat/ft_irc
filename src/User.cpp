#include "User.hpp"


User::User(){

}

User::User(std::string user_name, std::string nick_name, std::string real_name) {
	this->user_name = user_name;
	this->nick_name = nick_name;
	this->real_name = real_name;
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