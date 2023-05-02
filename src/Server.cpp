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
	std::string content = "";
	int i = 0;
    while (ss >> word) {
        if (i == 0 || word[0] != ':') {
			array.push_back(word);
		} else {
			content += word;
			break ;
		}
		++i;
    }
	while (ss >> word) {
		content += word;
	}
	array.push_back(content);



	// std::cout << "start of vector" << std::endl;
	i = 0;
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
	else if (array[0] == "PRIVMSG"){
		// PRIVMSG abullah :41\r\n
		// :osman 401 user_osman abullah :No such nick/channel\r\n
		// PRIVMSG abdullah :61\r\n
		// :user_osman!nick_os23@127.0.0.1 PRIVMSG abdullah :61\r\n /to abdullah
		
		if (array[1].substr(0,1) == "#")
			create_msg_2(pfds[index].fd, array[0] + " " + array[1] + " " + array[2]);	 // send_to_channel(array[1]); will change
		else if (is_nickname_exist(array[1]) != -1)
			send_to_user(index, array[0], array[1], array[2]);
		else
			send_msg(pfds[index].fd, create_msg(index, "401", array[1] + " :No such nick/channel"));

		//channel
		// PRIVMSG #aynen :knk\r\n
		// :abdullah!abdullah@127.0.0.1 PRIVMSG #aynen :knk\r\n
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

		// channeldaki her adam için ÇIKAN kişinin infosunu göndermek lazım 1 satır
		// :bmat!bmat@127.0.0.1 PART #aynen :bmat\r\n

		//yanlış yapı baştan göz gezdirilmesi lazım

		Channel channel = join_channel(array[1], index);
		send_msg(pfds[index].fd, create_msg_2(index, array[0] + " " + array[1])); 
		send_msg(pfds[index].fd, create_msg(index, "331", array[1] + " :" + channel.get_topic()));
		send_msg(pfds[index].fd, create_msg(index, "353", "= " + array[1] + " :" + channel.get_str_user_list()));//channelı name (array[1]) ile bulan fonksiyondan dönen channel objesinin tüm kullanıcıların ismini döndüren (arasında boşluk olacak şekilde) fonskiyonun çağır
		send_msg(pfds[index].fd, create_msg(index, "366", array[1] + " :End of NAMES list"));
		// channel.send_message()

	}
	else if (array[0] == "TOPIC"){
			
	}
	else if (array[0] == "ISON"){
			
	}
	else if (array[0] == "WHO"){
			//who join hemen sonrası atılır gelme şekli: WHO #<channel_name>
			//sadece o an giren kişiye yollanır diğerlerine özel tek satırlık Join mesajı yollanır

			// nick_osman tek başına odaya girdikten sonra gönderilecek kod parçası
			// :osman 352 nick_osman #aynen user_os23 127.0.0.1 osman nick_osman H :0 real_osyalcin\r\n:osman 315 nick_osman #aynen :End of WHO list\r\n

			//odada nick_osman varken bmat odaya girdikten sonra gönderilecek kod parçası
			// :osman 352 bmat #aynen user_os23 127.0.0.1 osman nick_osman H :0 real_osyalcin\r\n:osman 352 bmat #aynen bmat 127.0.0.1 osman bmat H :0 bmat\r\n:osman 315 bmat #aynen :End of WHO list\r\n
			
	}
	else if (array[0] == "PING"){ //MAYBE
			
	}
	else if (array[0] == "PART"){
		// :<nick_name>!<user_name>@<host_name> PART #<channel_name> :<nick_name>\r\n
		// :nick_osman!user_os23@127.0.0.1 PART #aynen :nick_osman\r\n
	}
	else if (array[0] == "QUIT")
	{
		/* code */
	}
}


void Server::send_to_user(int index, std::string command, std::string receiver_nick_name, std::string msg){
	int user_index = is_nickname_exist(receiver_nick_name);
	
	std::cout << "msg: " << create_msg_2(index, command + " " + receiver_nick_name + " " + msg) << std::endl;
	send_msg(pfds[user_index + 1].fd, create_msg_2(index, command + " " + receiver_nick_name + " " + msg));
}

std::string Server::create_msg(int index, std::string code, std::string msg) {
	return ":" + host_name + " " + code + " " + user_list[USER_ID].get_nick_name() + " " + msg + "\r\n";	

	// send_msg(pfds[index].fd, all_msg);
}

// :user_osman!nick_os23@127.0.0.1 PRIVMSG abdullah :61\r\n /to abdullah
std::string Server::create_msg_2(int sender, std::string msg){
	return ":"+ user_list[sender - 1].get_nick_name() + "!" + user_list[sender - 1].get_user_name() + "@" + host_name + " " + msg + "\r\n";

	// send_msg(pfds[receiver].fd, ":"+ user_list[sender - 1].get_nick_name() + "!" + user_list[sender - 1].get_user_name() + "@" + host_name + " " + msg + "\r\n");
}

void Server::send_msg(int fd, std::string msg) {
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

// Returns the index of the channel if it exists, -1 otherwise
int Server::is_channel_active(std::string channel) const
{
	int i = 0;
	for (std::vector<Channel>::const_iterator it = channel_list.begin(); it != channel_list.end(); ++it, ++i) {
		if ((*it).get_name() == channel)
			return i;
	}
	return -1;
}

Channel Server::find_channel(std::string name)
{
	for (std::vector<Channel>::iterator it = channel_list.begin(); it != channel_list.end(); ++it) {
		if ((*it).get_name() == name) {
			return (*it);
		}
	}
	throw std::exception();
}


Channel Server::join_channel(std::string channel, int index)
{
	int channel_index = is_channel_active(channel);
	if (channel_index != -1) {
		channel_list[channel_index].add_to_channel(user_list[USER_ID]);
		return channel_list[channel_index];
	} else {
		Channel new_channel(channel);
		new_channel.add_to_channel(user_list[USER_ID]);
		channel_list.push_back(new_channel);
		return new_channel;
	}
}

void Server::remove_from_all_channels(User user)
{
	for (std::vector<Channel>::iterator it = channel_list.begin(); it != channel_list.end(); ++it) {
		(*it).remove_from_channel(user);
	}
}
