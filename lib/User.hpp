#ifndef USER_HPP
#define USER_HPP
#include "Socket.hpp"

class User
{
	private:
		int fd;
		std::string user_name;
		std::string nick_name;
		std::string real_name;

	public:
		User();
		User(std::string user_name, std::string nick_name, std::string real_name, int fd);
		~User();

		std::string get_user_name() const;
		std::string get_nick_name() const;
		std::string get_real_name() const;
		int get_fd() const;
};


#endif