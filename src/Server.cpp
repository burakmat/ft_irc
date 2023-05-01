#include "Server.hpp"

Server::Server(int port) : Socket::Socket(port)
{
	fail_check(bind(fd, (struct sockaddr *)&address, sizeof(address)));
	fail_check(listen(fd, MAX_CLIENT));
	create_fd(fd);
	char tmp[255];
	gethostname(tmp, 255);
	host_name = tmp;


	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	std::time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
	created_time = std::string(buffer);
}

Server::~Server()
{

}

void Server::acception()
{
	int client_fd = accept(fd, 0, 0);
	fail_check(client_fd);
	create_fd(client_fd);
}

void Server::create_user(std::string user_name, std::string nick_name, std::string real_name){

	User new_user(user_name, nick_name, real_name);
	user_list.push_back(new_user);
}

void Server::delete_user(){
}

void Server::create_fd(int fd){
	struct pollfd temp;
	temp.fd = fd;
	temp.events = POLLIN;
	temp.revents = 0;

	pfds.push_back(temp);
}

void Server::delete_fd(int index){
	//delete index
	close(pfds[index].fd);
	pfds.erase(pfds.begin() + index);
}

void Server::getting_command(int index, std::string buffer){

    std::vector<std::string> array;
    std::stringstream ss(buffer);

    std::string word;
    while (ss >> word) {
        array.push_back(word);
    }

		std::cout << "start of vector" << std::endl;
		int i = 0;
		for (std::vector<std::string>::iterator it = array.begin(); it != array.end(); ++it) {
        std::cout << "i :" <<  i << "value : "  << *it << std::endl;
				i++;
    }
		std::cout << "end of vector" << std::endl;



		if (array[0] == "USER")
		{
			create_user(array[6], array[1], array[4].substr(1));
			create_msg(index, "001", user_list[index].get_user_name() + " :Hi, welcome to IRC");
			create_msg(index, "002", user_list[index].get_user_name() + " :Your host is " + host_name + ", running version v1");
			create_msg(index, "003", user_list[index].get_user_name() + " :This server was created " + created_time);
			create_msg(index, "004", user_list[index].get_user_name() + " " + host_name + " v1 o o");
			create_msg(index, "251", user_list[index].get_user_name() + " :There are " + std::to_string(user_list.size()) + " users and 0 services on 1 server");
		}
		else if (array[0] == "PRIVMSG"){
			
		}
		else if (array[0] == "JOIN")
		{
			// ":mat!mat@192.168.1.34 JOIN mati\r\n";
			// ":buraks-mbp.home 331 mat mati :No topic is set\r\n";
			// ":buraks-mbp.home 353 mat = mati :mat\r\n";
			// ":buraks-mbp.home 366 mat mati :End of NAMES list\r\n";

			send_msg(pfds[index].fd, ":"+ user_list[index].get_user_name() + "!" + user_list[index].get_user_name() + "@" + host_name +" JOIN " + array[1] +"\r\n");
			create_msg(index, "331", user_list[index].get_user_name() + " " + array[1] + " :No topic is set");
			create_msg(index, "353", user_list[index].get_user_name() + " = " + array[1] + " :osman");//will change
			create_msg(index, "366", user_list[index].get_user_name() + " " + array[1] + " :End of NAMES list");
		}
		else if (array[0] == "QUIT")
		{
			/* code */
		}		
}

void Server::create_msg(int index, std::string code, std::string msg){
	std::string all_msg = ":" + host_name + " " + code + " " + msg + "\r\n";	

	send_msg(pfds[index].fd, all_msg);
}

void Server::send_msg(int fd, std::string msg){
	write(fd, msg.c_str(), msg.length());
}

std::string Server::get_host_name(){
	return host_name;
}