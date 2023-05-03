#include "Server.hpp"

Server::Server(int port) : Socket::Socket(port)
{
	fail_check(bind(fd, (struct sockaddr *)&address, sizeof(address)));
	fail_check(listen(fd, MAX_CLIENT));
	create_fd(fd);

	//host_name
	char tmp[255];
	gethostname(tmp, 255);
	host_name = tmp;

	//time
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

void Server::create_user(std::string user_name, std::string nick_name, std::string real_name, int fd){
	if (is_nickname_exist(nick_name) == -1) {
		User new_user(user_name, nick_name, real_name, fd);
		user_list.push_back(new_user);
	} else {
		// send error code
	}
}

void Server::delete_user(int index) {
		user_list.erase(user_list.begin() + USER_ID);
}

void Server::create_fd(int fd) {
	struct pollfd temp;
	temp.fd = fd;
	temp.events = POLLIN;
	temp.revents = 0;

	pfds.push_back(temp);
}

void Server::delete_fd(int index) {
	//delete index
	close(pfds[index].fd);
	pfds.erase(pfds.begin() + index);
}

void Server::getting_command(int index, std::string buffer) {

    std::vector<std::string> array;

		std::stringstream ss(buffer);
		std::string word;
		while (ss >> word)
		{
			array.push_back(word);
		}
		
	int i = 0;
	// std::cout << "start of vector" << std::endl;
	for (std::vector<std::string>::iterator it = array.begin(); it != array.end(); ++it) {
		std::cout << "i:" << i << ", value: " << *it << std::endl;
		i++;
	}
	// std::cout << "end of vector" << std::endl;

	if (array[0] == "USER")
	{
		// :osman 001 osman :Hi, welcome to IRC\r\n
		// :osman 002 osman :Your host is osman, running version miniircd-2.2\r\n
		// :osman 003 osman :This server was created sometime\r\n
		// :osman 004 osman osman miniircd-2.2 o o\r\n
		// :osman 251 osman :There are 1 users and 0 services on 1 server\r\n

		create_user(array[1], array[6], array[4].substr(1), pfds[index].fd);
		send_msg(pfds[index].fd, create_msg(index, "001", ":Hi, welcome to IRC"));
		send_msg(pfds[index].fd, create_msg(index, "002", ":Your host is " + host_name + ", running version v1"));
		send_msg(pfds[index].fd, create_msg(index, "003", ":This server was created " + created_time));
		send_msg(pfds[index].fd, create_msg(index, "004", host_name + " v1 o o"));
		send_msg(pfds[index].fd, create_msg(index, "251", ":There are " + std::to_string(user_list.size()) + " users and 0 services on 1 server"));
	}
	else if (array[0] == "PRIVMSG" || array[0] == "NOTICE"){
		// user
		// PRIVMSG abullah :41\r\n
		// :osman 401 user_osman abullah :No such nick/channel\r\n
		// PRIVMSG abdullah :61\r\n
		// :user_osman!nick_os23@127.0.0.1 PRIVMSG abdullah :61\r\n /to abdullah
		
		// channel
		// PRIVMSG #aynen :sdadsa\r\n 
		// :abdullah!abdullah@127.0.0.1 PRIVMSG #aynen :sdadsa\r\n' to other user
		// :abdullah!abdullah@127.0.0.1 PRIVMSG #aynen :sdadsa\r\n to other user

		if (array[1].substr(0,1) == "#") //channel-user
			find_channel(array[1]).send_message(user_list[USER_ID], create_msg_2(index, array[0] + " " + array[1] + " " + array[2]));
		else if (is_nickname_exist(array[1]) != -1) //user-user
			user_to_user(index, array[0], array[1], array[2]);
		else
			send_msg(pfds[index].fd, create_msg(index, "401", array[1] + " :No such nick/channel"));

	}
	else if (array[0] == "JOIN")
	{
		//	first join message
		// ":abdullah!abdullah@192.168.1.34 JOIN #aynen\r\n"; 
		// ":buraks-mbp.home 331 mat #mati :No topic is set\r\n";
		// ":buraks-mbp.home 353 mat = #mati :mat\r\n";
		// ":buraks-mbp.home 366 mat #mati :End of NAMES list\r\n";
		// gelen kişi JOIN sonrası WHO mesaji gönderir

		// channeldaki her adam için GELEN kişinin infosunu göndermek lazım 1 satır
		// ":abdullah!abdullah@192.168.1.34 JOIN #aynen\r\n"; to everyone

		int channel_index = is_channel_active(array[1]);
		if (channel_list[channel_index].user_exists(user_list[USER_ID]))
			return ;
		channel_list[channel_index].add_to_channel(user_list[USER_ID]);
		send_msg(pfds[index].fd, create_msg_2(index, array[0] + " " + array[1])); 
		send_msg(pfds[index].fd, create_msg(index, "331", array[1] + " :" + channel_list[channel_index].get_topic()));
		send_msg(pfds[index].fd, create_msg(index, "353", "= " + array[1] + " :" + channel_list[channel_index].get_str_user_list()));
		send_msg(pfds[index].fd, create_msg(index, "366", array[1] + " :End of NAMES list"));
		channel_list[channel_index].send_message(user_list[USER_ID], create_msg_2(index, array[0] + " " + array[1]));
	}
	else if (array[0] == "TOPIC"){
		Channel channel = find_channel(array[1]);
		channel.set_topic(array[2].substr(1));
		std::string message = create_msg_2(index, "TOPIC " + array[1] + " " + array[2]);
		channel.send_message(user_list[USER_ID], message);
		write(user_list[USER_ID].get_fd(), message.c_str(), message.length());
	}
	else if (array[0] == "ISON"){
			
	}
	else if (array[0] == "WHO"){
			//who join hemen sonrası atılır gelme şekli: WHO #<channel_name>
			//sadece o an giren kişiye yollanır diğerlerine özel tek satırlık Join mesajı yollanır

			// nick_osman tek başına odaya girdikten sonra gönderilecek kod parçası
			// :osman 352 nick_osman #aynen user_os23 127.0.0.1 osman nick_osman H :0 real_osyalcin\r\n
			// :osman 315 nick_osman #aynen :End of WHO list\r\n

			//odada nick_osman varken abdullah odaya girdikten sonra gönderilecek kod parçası
			// :osman 352 abdullah #aynen user_os23 127.0.0.1 osman nick_osman H :0 real_osyalcin\r\n 
			// :osman 352 abdullah #aynen abdullah 127.0.0.1 osman abdullah H :0 abdullah\r\n
			// :osman 315 abdullah #aynen :End of WHO list\r\n

			Channel channel = find_channel(array[1]);
		 	std::vector<User> channels_user = channel.get_user_list();
			for (std::vector<User>::const_iterator it = channels_user.begin(); it != channels_user.end(); ++it) {
				send_msg(pfds[index].fd, create_msg(index, "352", (*it).get_user_name() + " " + host_name + " " + host_name + " " + (*it).get_nick_name() + " H :0 " + (*it).get_real_name())); //H AND :0 could change
			}
			send_msg(pfds[index].fd, create_msg(index, "315", array[1] + " :End of WHO list"));
	}
	else if (array[0] == "PING"){ //MAYBE
			
	}
	else if (array[0] == "PART"){
		// PART #aynen\r\n'
		// :nick_osman!user_os23@127.0.0.1 PART #aynen :nick_osman\r\n'
		// :nick_osman!user_os23@127.0.0.1 PART #aynen :nick_osman\r\n'
		// :nick_osman!user_os23@127.0.0.1 PART #aynen :nick_osman\r\n'

		find_channel(array[1]).remove_from_channel(user_list[USER_ID]);
		std::vector<User> channels_user = find_channel(array[1]).get_user_list();
		for (std::vector<User>::const_iterator it = channels_user.begin(); it != channels_user.end(); ++it) {
			send_msg((*it).get_fd(), create_msg_2(index, array[0] + " " + array[1] + " :" + user_list[USER_ID].get_nick_name()));
		}
	}
	else if (array[0] == "QUIT")
	{
		// send to all user
		// nick_osman!user_os23@127.0.0.1 QUIT :Leaving.\r\n
		remove_from_all_channels(user_list[USER_ID], index);
		delete_fd(index);
		delete_user(index);
	}
}

std::vector<std::string> Server::parse(std::string input) {
	std::vector<std::string> vec, result;
	std::stringstream ss(input);
	std::string temp = "";

	while (getline(ss, temp)) {
		vec.push_back(temp);
	}

	ss.str(input);
	ss.clear();
	
	if (vec.size() >= 2) {
		while (ss >> temp) {
			result.push_back(temp);
		}
	} else {
		temp = input.substr(0, input.find_last_of(':'));
		ss.str(temp);
		ss.clear();
		while (ss >> temp) {
			result.push_back(temp);
		}
		result.push_back(input.substr(input.find_last_of(':')));
	}
	return result;
}



void Server::user_to_user(int index, std::string command, std::string receiver_nick_name, std::string msg){
	int user_index = is_nickname_exist(receiver_nick_name);
	
	std::cout << "msg: " << create_msg_2(index, command + " " + receiver_nick_name + " " + msg) << std::endl;
	send_msg(pfds[user_index + 1].fd, create_msg_2(index, command + " " + receiver_nick_name + " " + msg));
}

std::string Server::create_msg(int index, std::string code, std::string msg) {
	return ":" + host_name + " " + code + " " + user_list[USER_ID].get_nick_name() + " " + msg + "\r\n";	
}

std::string Server::create_msg_2(int index, std::string msg){ //index = sender
	return ":"+ user_list[USER_ID].get_nick_name() + "!" + user_list[USER_ID].get_user_name() + "@" + host_name + " " + msg + "\r\n";
}

void Server::send_msg(int fd, std::string msg) {
	std::cout << "sending back: " << msg ;
	write(fd, msg.c_str(), msg.length());
}

std::string Server::get_host_name() {
	return host_name;
}

int Server::is_nickname_exist(std::string nick) const
{
	int i = 0;
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		if ((*it).get_nick_name() == nick)
			return i;
		++i;
	}
	return -1;
}

int Server::is_channel_active(std::string channel)
{
	int i = 0;
	for (std::vector<Channel>::const_iterator it = channel_list.begin(); it != channel_list.end(); ++it, ++i) {
		if ((*it).get_name() == channel)
			return i;
	}
	Channel new_channel(channel);
	channel_list.push_back(new_channel);
	return i;
}

Channel &Server::find_channel(std::string name)
{
	for (std::vector<Channel>::iterator it = channel_list.begin(); it != channel_list.end(); ++it) {
		if ((*it).get_name() == name) {
			return (*it);
		}
	}
	throw std::exception();
}

void Server::remove_from_all_channels(User user, int index)
{
	for (std::vector<Channel>::iterator it = channel_list.begin(); it != channel_list.end(); ++it) {
		if ((*it).user_exists(user)) {
			(*it).send_message(user, create_msg_2(index, "QUIT :Leaving."));
		}
		(*it).remove_from_channel(user);
	}
}

// DEBUG
void Server::print_info() const {
	int i = 0;
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it) {
		std::cout << "User " << i << ": " << (*it).get_nick_name() << ", fd in its class: " << (*it).get_fd() << std::endl;
		++i;
	}
	i = 0;
	for (std::vector<struct pollfd>::const_iterator it = pfds.begin(); it != pfds.end(); ++it) {
		std::cout << "Server fd " << i << ": " << (*it).fd << std::endl;
		++i;
	}
}