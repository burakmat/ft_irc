#ifndef USER_HPP
#define USER_HPP
#include "Socket.hpp"

class User
{
private:
	std::string user_name;
	std::string nick_name;
	std::string real_name;

public:
	User();
	User(std::string user_name, std::string nick_name, std::string real_name);
	~User();

	std::string get_user_name();
	std::string get_nick_name();
	std::string get_real_name();
};


#endif