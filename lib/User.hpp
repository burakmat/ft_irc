#ifndef USER_HPP
#define USER_HPP
#include "Socket.hpp"

class User
{
	private:
		int fd;
		bool verified;
		std::string user_name;
		std::string nick_name;
		std::string real_name;

	public:
		User(int _fd);
		~User();

		std::string get_user_name() const;
		std::string get_nick_name() const;
		std::string get_real_name() const;
		bool is_verified() const;
		void set_verified(bool truth);
		void set_names(std::string _user_name, std::string _nick_name, std::string _real_name);
		int get_fd() const;
};


#endif