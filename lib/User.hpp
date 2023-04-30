#ifndef USER_HPP
#define USER_HPP
#include "Socket.hpp"

class User
{
private:
	int id;
	std::string user_name;
	std::string nick_name;
	std::string real_name;

public:
	User();
	User(int id, std::string buffer);
	~User();

	std::string get_user_name();
};


#endif