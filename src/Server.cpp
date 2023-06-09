#include "Server.hpp"

Server::Server(int port, std::string _password) 
: Socket::Socket(port),
password(_password)
{
	fail_check(bind(fd, (struct sockaddr *)&address, sizeof(address)));
	fail_check(listen(fd, MAX_CLIENT));
	create_fd(fd);
	set_host_name();
	set_time();
	std::cout << "PASSWORD: '" + password + "'" << std::endl;
	
}

Server::~Server() {}

void Server::acception()
{
	int client_fd = accept(fd, 0, 0);
	fail_check(client_fd);
	create_fd(client_fd);
	User new_user(client_fd);
	user_list.push_back(new_user);
}

void Server::getting_command(int index, std::string buffer)
{

	std::vector<std::string> array;
	array = parse(buffer);

	if (array.size() == 0)
		return ;
	
	int i = 0;

	for (std::vector<std::string>::iterator it = array.begin(); it != array.end(); ++it)
	{
		std::cout << "i:" << i << ", value: " << *it << std::endl;
		i++;
	}

	if (array[0] == "PASS" || array[0] == "USER" || array[0] == "NICK")
		command_user(index, array);
	else if (user_list[USER_ID].is_verified() && user_list[USER_ID].get_nick_name() != "" && user_list[USER_ID].get_user_name() != "")
	{
		if (array[0] == "PRIVMSG" || array[0] == "NOTICE")
			command_privmsg(index, array);
		else if (array[0] == "JOIN")
			command_join(index, array);
		else if (array[0] == "TOPIC")
			command_topic(index, array);
		else if (array[0] == "WHO")
			command_who(index, array);
		else if (array[0] == "PART")
			command_part(index, array);
		else if (array[0] == "QUIT")
			command_quit(index);
		else if (array[0] == "LIST")
		{
			for (std::vector<Channel>::const_iterator it = channel_list.begin(); it != channel_list.end(); ++it)
			{
				send_msg(pfds[index].fd, create_msg(index, "322", (*it).get_name() + " " + std::to_string((*it).get_user_list().size()) + " :" + (*it).get_topic()));
			}
			send_msg(pfds[index].fd, create_msg(index, "323", ":End of LIST"));
		}
		else if (array[0] == "PING")
		{
			send_msg(pfds[index].fd, ":" + get_host_name() + " PONG " + get_host_name() + " :" + array[1] + "\r\n");
		}
		else if (array[0] == "NAMES")
		{
			int channel_index = is_channel_active(array[1], user_list[USER_ID].get_nick_name());
			send_msg(pfds[index].fd, create_msg(index, "353", "= " + array[1] + " :" + channel_list[channel_index].get_str_user_list()));
			send_msg(pfds[index].fd, create_msg(index, "366", array[1] + " :End of NAMES list"));
		}
		else if (array[0] == "AWAY")
		{
			return;
		}
		else if (array[0] == "INVITE")
		{
			if (array.size() < 3)
			{
				send_msg(pfds[index].fd, create_msg(index, "461", array[0] + " :Not enough parameters"));
				return;
			}

			int channel_index = is_channel_active_v2(array[2]);
			if (channel_index == -1)
			{
				send_msg(pfds[index].fd, create_msg(index, "403", array[1] + " :No such channel"));
				return;
			}

			if (!channel_list[channel_index].is_mode_users(user_list[USER_ID].get_nick_name()))
			{
				send_msg(pfds[index].fd, create_msg(index, "482", ":You're not channel operator"));
				return;
			}

			int target_user_index = is_nickname_exist(array[1]);
			if (target_user_index == -1)
			{
				send_msg(pfds[index].fd, create_msg(index, "401", array[1] + " :No such nick"));
				return ;
			}
			
			channel_list[channel_index].add_invite(user_list[target_user_index].get_nick_name());
			send_msg(pfds[target_user_index + 1].fd, create_msg_2(index, array[0] + " " + array[1] + " " + array[2]));
		}
		else if (array[0] == "KICK")
		{
			if (array.size() < 3)
			{
				send_msg(pfds[index].fd, create_msg(index, "461", array[0] + " :Not enough parameters"));
				return;
			}

			int channel_index = is_channel_active_v2(array[1]);

			if (channel_index == -1)
			{
				send_msg(pfds[index].fd, create_msg(index, "403", array[1] + " :No such channel"));
				return;
			}

			if (!channel_list[channel_index].is_mode_users(user_list[USER_ID].get_nick_name()))
			{
				send_msg(pfds[index].fd, create_msg(index, "482", ":You're not channel operator"));
				return;
			}

			if (!channel_list[channel_index].user_exists_name(array[2]))
			{
				send_msg(pfds[index].fd, create_msg(index, "401", array[2] + " :No such nick"));
				return;
			}

			if (channel_list[channel_index].get_creator() == array[2])
			{
				send_msg(pfds[index].fd, create_msg(index, "423", ":You can not kick creator of channel"));
				return;
			}

			std::vector<User> channels_user = find_channel(array[1]).get_user_list();
			for (std::vector<User>::const_iterator it = channels_user.begin(); it != channels_user.end(); ++it)
			{
				if ((*it).get_nick_name() == array[2])
				{
					find_channel(array[1]).remove_from_channel((*it));
					if (find_channel(array[1]).get_user_list().size() == 0)
						channel_list.erase(channel_list.begin() + channel_index);
				}
				if (array.size() < 4)
				{
					send_msg((*it).get_fd(), create_msg_2(index, array[0] + " " + array[1] + " " + array[2] + " :"));
				}
				else
				{
					send_msg((*it).get_fd(), create_msg_2(index, array[0] + " " + array[1] + " " + array[2] + " :" + array[3]));
				}
			}
		}

		else if (array[0] == "MODE")
		{
			if (array.size() < 3)
			{
				send_msg(pfds[index].fd, create_msg(index, "461", array[1] + " MODE :Not enough parameters"));
				return;
			}

			int channel_index = is_channel_active_v2(array[1]);

			if (channel_index == -1)
			{
				send_msg(pfds[index].fd, create_msg(index, "403", array[1] + " :No such channel"));
				return;
			}

			if (!channel_list[channel_index].user_exists_name(user_list[USER_ID].get_nick_name()))
			{
				send_msg(pfds[index].fd, create_msg(index, "442", array[1] + " :You're not on that channel"));
				return;
			}

			if (!channel_list[channel_index].is_mode_users(user_list[USER_ID].get_nick_name()))
			{
				send_msg(pfds[index].fd, create_msg(index, "482", ":You're not channel operator"));
				return;
			}

			if (array[2] == "+k")
			{
				if (array.size() < 4)
				{
					send_msg(pfds[index].fd, create_msg(index, "461", array[1] + " MODE :Not enough parameters"));
					return;
				}
				channel_list[channel_index].send_message(user_list[USER_ID], create_msg_2(index, "MODE " + array[1] + " " + array[2] + " " + array[3]), true);
				channel_list[channel_index].add_mode("+k");
				channel_list[channel_index].set_password(array[3]);
			}
			else if (array[2] == "-k")
			{
				channel_list[channel_index].send_message(user_list[USER_ID], create_msg_2(index, "MODE " + array[1] + " " + array[2]), true);
				channel_list[channel_index].remove_mode("+k");
				channel_list[channel_index].set_password("");
			}
			else if (array[2] == "+o")
			{
				if (array.size() < 4)
				{
					send_msg(pfds[index].fd, create_msg(index, "461", array[1] + " MODE :Not enough parameters"));
					return;
				}

				if (!channel_list[channel_index].user_exists_name(array[3]))
				{
					send_msg(pfds[index].fd, create_msg(index, "401", array[3] + " :No such nick/channel"));
				}
				else
				{
					channel_list[channel_index].send_message(pfds[index].fd, create_msg_2(index, "MODE " + array[1] + " " + array[2] + " " + array[3]), true);
					channel_list[channel_index].add_mode_users(array[3]);
				}
			}
			else if (array[2] == "-o")
			{
				if (channel_list[channel_index].get_creator() == array[3])
				{
					send_msg(pfds[index].fd, create_msg(index, "423", ":You can not remove creator of channel"));
					return;
				}

				if (!channel_list[channel_index].user_exists_name(array[3]))
				{
					send_msg(pfds[index].fd, create_msg(index, "401", array[3] + " :No such nick/channel"));
				}
				else
				{
					channel_list[channel_index].send_message(pfds[index].fd, create_msg_2(index, "MODE " + array[1] + " " + array[2] + " " + array[3]), true);
					channel_list[channel_index].remove_mode_users(array[3]);
				}
			}
			else if (array[2] == "+i")
			{
				channel_list[channel_index].add_mode("+i");
				channel_list[channel_index].send_message(pfds[index].fd, create_msg_2(index, "MODE " + array[1] + " " + array[2]), true);
			}
			else if (array[2] == "-i")
			{
				channel_list[channel_index].remove_mode("+i");
				channel_list[channel_index].send_message(pfds[index].fd, create_msg_2(index, "MODE " + array[1] + " " + array[2]), true);
			}
			else {
				send_msg(pfds[index].fd, create_msg(index, "501", array[2] + " :Unknown MODE flag"));
			}
		}
		else
			send_msg(pfds[index].fd, create_msg(index, "421", array[0] + " :Unknown command"));
	}
}

std::vector<std::string> Server::parse(std::string input)
{
	std::vector<std::string> vec, result;
	std::stringstream ss(input);
	std::string temp = "";

	ss >> temp;
	if (temp == "") {
		return result;
	}
	ss.str(input);
	ss.clear();

	while (getline(ss, temp))
	{
			vec.push_back(temp);
	}

	ss.str(input);
	ss.clear();

	if (vec.size() >= 2)
	{
		while (ss >> temp)
		{
			result.push_back(temp);
		}
	}
	else
	{
		if (input.find_last_of(':') == std::string::npos)
		{
			ss.str(input);
			ss.clear();
			while (ss >> temp)
			{
				result.push_back(temp);
			}
			return result;
		}
		temp = input.substr(0, input.find_last_of(':'));
		ss.str(temp);
		ss.clear();
		while (ss >> temp)
		{
			result.push_back(temp);
		}
		if (input.find_last_of('\r') != std::string::npos)
			temp = input.substr(input.find_last_of(':'), input.find_last_of('\r') - input.find_last_of(':'));
		else if (input.find_last_of('\n') != std::string::npos)
			temp = input.substr(input.find_last_of(':'), input.find_last_of('\n') - input.find_last_of(':'));
		else
			temp = input.substr(input.find_last_of(':'));

		result.push_back(temp);
	}
	return result;
}

void Server::user_to_user(int index, std::string command, std::string receiver_nick_name, std::string msg)
{
	int user_index = is_nickname_exist(receiver_nick_name);

	send_msg(pfds[user_index + 1].fd, create_msg_2(index, command + " " + receiver_nick_name + " " + msg));
}

std::string Server::create_msg(int index, std::string code, std::string msg)
{
	return ":" + host_name + " " + code + " " + user_list[USER_ID].get_nick_name() + " " + msg + "\r\n";
}

std::string Server::create_msg_2(int index, std::string msg)
{
	return ":" + user_list[USER_ID].get_nick_name() + "!" + user_list[USER_ID].get_user_name() + "@" + host_name + " " + msg + "\r\n";
}

void Server::send_msg(int fd, std::string msg)
{
	std::cout << "sending back: " << msg;
	write(fd, msg.c_str(), msg.length());
}

int Server::verified_size() const
{
	int counter = 0;
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it)
	{
		if ((*it).is_verified())
		{
			++counter;
		}
	}
	return counter;
}

int Server::is_nickname_exist(std::string nick) const
{
	int i = 0;
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it)
	{
		if ((*it).get_nick_name() == nick)
			return i;
		++i;
	}
	return -1;
}

// CHANNEL_METHODS

int Server::is_channel_active(std::string channel, std::string user_name)
{
	int i = 0;
	for (std::vector<Channel>::const_iterator it = channel_list.begin(); it != channel_list.end(); ++it, ++i)
	{
		if ((*it).get_name() == channel)
			return i;
	}
	Channel new_channel(channel);
	new_channel.set_creator(user_name);
	new_channel.add_mode_users(user_name);
	channel_list.push_back(new_channel);
	return i;
}

int Server::is_channel_active_v2(std::string channel)
{
	int i = 0;
	for (std::vector<Channel>::const_iterator it = channel_list.begin(); it != channel_list.end(); ++it, ++i)
	{
		if ((*it).get_name() == channel)
			return i;
	}
	return -1;
}

Channel &Server::find_channel(std::string name)
{
	for (std::vector<Channel>::iterator it = channel_list.begin(); it != channel_list.end(); ++it)
	{
		if ((*it).get_name() == name)
		{
			return (*it);
		}
	}
	throw std::exception();
}

void Server::remove_from_all_channels(User user, int index)
{
	std::vector<int> tmp;
	int i = 0;
	for (std::vector<Channel>::iterator it = channel_list.begin(); it != channel_list.end(); ++it)
	{
		if ((*it).user_exists(user))
		{
			(*it).send_message(user, create_msg_2(index, "QUIT :Leaving."), false);
		}
		(*it).remove_from_channel(user);
		if ((*it).get_user_list().size() == 0)
			tmp.push_back(i);
		i++;
	}
	std::sort(tmp.begin(), tmp.end());
	int tmp_size = tmp.size();
	if (tmp_size > 0)
	{
		for (int i = tmp_size - 1; i >= 0; --i)
		{
			channel_list.erase(channel_list.begin() + tmp[i]);
		}
	}
}

int Server::read_init_command(std::vector<std::string> array, int index)
{
	unsigned long i = 0;
	while (i < array.size()) {
		if (array[i] == "PASS") {
			
	std::cout << "current password: '" << password << "', " << "given PASSWORD: '" + array[i + 1].substr(1) + "'" << std::endl;

		}
		if (array[i] == "PASS" && array[i + 1].substr(1) == password) {
			user_list[USER_ID].set_verified(true);
			i += 1;
		}
		else if (user_list[USER_ID].is_verified()) {
			if (array[i] == "NICK") {
				if (array[i + 1][0] == ':') {
					// Cannot start with number
					if (array[i + 1].size() >= 2 && isdigit(array[i + 1][1])) {
						std::string message = array[i + 1].substr(1) + " :Erroneous Nickname";
						send_msg(user_list[USER_ID].get_fd(), create_msg(user_list[USER_ID].get_fd(), "432", message));
						return 1;
					} else if (is_nickname_exist(array[i + 1].substr(1)) != -1) {
						std::string message = array[i + 1].substr(1) + " :Nickname is already in use";
						send_msg(user_list[USER_ID].get_fd(), create_msg(user_list[USER_ID].get_fd(), "433", message));
						return 1;
					}
					// Should inform all channel members
					for (std::vector<Channel>::iterator it = channel_list.begin(); it != channel_list.end(); ++it) {
						if ((*it).user_exists_name(user_list[USER_ID].get_nick_name())) {
							(*it).send_message(user_list[USER_ID], create_msg_2(index, "NICK " + array[i + 1].substr(1)), true);
							
							User user = user_list[USER_ID];
							user.set_nick_name(array[i + 1].substr(1));
							(*it).user_list.insert((*it).user_list.begin(), user);
							(*it).add_mode_users(array[i + 1].substr(1));
							
							(*it).remove_from_channel(user_list[USER_ID]);
							if (user_list[USER_ID].get_nick_name() == (*it).get_creator()) {
								(*it).set_creator(array[i + 1].substr(1));
							}
							
						}
					}
					user_list[USER_ID].set_nick_name(array[i + 1].substr(1));
					return 1;
				} else {
					if (i + 1 < array.size() && isdigit(array[i + 1][0])) {
						std::string message = array[i + 1] + " :Erroneous Nickname";
						send_msg(user_list[USER_ID].get_fd(), create_msg(user_list[USER_ID].get_fd(), "432", message));
						return 1;
					} else if (is_nickname_exist(array[i + 1]) != -1) {
						std::string message = array[i + 1] + " :Nickname is already in use";
						send_msg(user_list[USER_ID].get_fd(), create_msg(user_list[USER_ID].get_fd(), "433", message));
						return 1;
					}
					user_list[USER_ID].set_nick_name(array[i + 1]);
				}
				i += 1;
			}
			else if (array[i] == "USER") {
				user_list[USER_ID].set_user_name(array[i + 1]);
				user_list[USER_ID].set_real_name(array[i + 4].substr(1));
				i += 4;
			}
		}
		++i;
	}
	return 0;
}



// PRIVATE FUNCTIONS
void Server::command_user(int index, std::vector<std::string> array)
{

	if (read_init_command(array, index) == 1) {
		return ;
	}

	if (user_list[USER_ID].is_verified() && (user_list[USER_ID].get_nick_name() == "" || user_list[USER_ID].get_user_name() == "")) {
		// Waits for NICK or USER
	}
	else if (user_list[USER_ID].is_verified() && user_list[USER_ID].get_nick_name() != "" && user_list[USER_ID].get_real_name() != "" && user_list[USER_ID].get_user_name() != "") {
		send_msg(pfds[index].fd, create_msg(index, "001", ":Hi, welcome to IRC"));
		send_msg(pfds[index].fd, create_msg(index, "002", ":Your host is " + host_name + ", running version v1"));
		send_msg(pfds[index].fd, create_msg(index, "003", ":This server was created " + created_time));
		send_msg(pfds[index].fd, create_msg(index, "004", host_name + " v1 o o"));
		send_msg(pfds[index].fd, create_msg(index, "251", ":There are " + std::to_string(verified_size()) + " users and 0 services on 1 server"));
	} else {
		std::string error_message = ":" + host_name + " 464 :Password incorrect\r\n";
		send_msg(pfds[index].fd, error_message);
	}
}

void Server::command_privmsg(int index, std::vector<std::string> array)
{
	
	if (array[1].substr(0, 1) == "#") { // channel-user
		int i = is_channel_active_v2(array[1]);
		if (i == -1)
			return ;
		
		if (!channel_list[i].user_exists_name(user_list[USER_ID].get_nick_name())){
			return;
		}

		find_channel(array[1]).send_message(user_list[USER_ID], create_msg_2(index, array[0] + " " + array[1] + " " + array[2]), false);
	}
	else if (is_nickname_exist(array[1]) != -1) // user-user
		user_to_user(index, array[0], array[1], array[2]);
	else
		send_msg(pfds[index].fd, create_msg(index, "401", array[1] + " :No such nick/channel"));
}

void Server::command_join(int index, std::vector<std::string> array)
{
	if (array[1][0] != '#')
		array[1] = "#" + array[1];

	int channel_index = is_channel_active(array[1], user_list[USER_ID].get_nick_name());
	if (channel_list[channel_index].user_exists(user_list[USER_ID]))
		return;

	std::cout << "if check: " << channel_list[channel_index].is_exist_mode("+k") << "size: " << array.size() << "pass: " << channel_list[channel_index].get_password() << std::endl;
	if (channel_list[channel_index].is_exist_mode("+i") && !channel_list[channel_index].is_invited(user_list[USER_ID].get_nick_name())) {
		send_msg(pfds[index].fd, create_msg(index, "473", channel_list[channel_index].get_name() + " :Cannot join channel(+i)"));
		return ;
	} else if (channel_list[channel_index].is_invited(user_list[USER_ID].get_nick_name())) {
		channel_list[channel_index].remove_invite(user_list[USER_ID].get_nick_name());
	} else if (!channel_list[channel_index].is_exist_mode("+i") && channel_list[channel_index].is_exist_mode("+k") && (array.size() < 3 || array[2] != channel_list[channel_index].get_password()))
	{
		send_msg(pfds[index].fd, create_msg(index, "475", array[1] + " :Cannot join channel (+k) - bad key"));
		return;
	}

	channel_list[channel_index].add_to_channel(user_list[USER_ID]);
	send_msg(pfds[index].fd, create_msg_2(index, array[0] + " " + array[1]));
	send_msg(pfds[index].fd, create_msg(index, "331", array[1] + " :" + channel_list[channel_index].get_topic()));
	send_msg(pfds[index].fd, create_msg(index, "353", "= " + array[1] + " :" + channel_list[channel_index].get_str_user_list()));
	send_msg(pfds[index].fd, create_msg(index, "366", array[1] + " :End of NAMES list"));
	channel_list[channel_index].send_message(user_list[USER_ID], create_msg_2(index, array[0] + " " + array[1]), false);
}

void Server::command_topic(int index, std::vector<std::string> array)
{
	int channel_index = is_channel_active_v2(array[1]);
	if (channel_index == -1)
		return ;
	channel_list[channel_index].set_topic(array[2].substr(1));
	std::string message = create_msg_2(index, "TOPIC " + array[1] + " " + array[2]);
	channel_list[channel_index].send_message(user_list[USER_ID], message, true);
}

void Server::command_who(int index, std::vector<std::string> array)
{
	int channel_index = is_channel_active_v2(array[1]);
	if (channel_index == -1)
		return;
	
	std::vector<User> channels_user = channel_list[channel_index].get_user_list();
	for (std::vector<User>::const_iterator it = channels_user.begin(); it != channels_user.end(); ++it)
	{
		send_msg(pfds[index].fd, create_msg(index, "352", (*it).get_user_name() + " " + host_name + " " + host_name + " " + (*it).get_nick_name() + " H :0 " + (*it).get_real_name())); // H AND :0 could change
	}
	send_msg(pfds[index].fd, create_msg(index, "315", array[1] + " :End of WHO list"));

}

void Server::command_part(int index, std::vector<std::string> array)
{
	std::vector<User> channels_user = find_channel(array[1]).get_user_list();
	for (std::vector<User>::const_iterator it = channels_user.begin(); it != channels_user.end(); ++it) {
		send_msg((*it).get_fd(), create_msg_2(index, array[0] + " " + array[1] + " :" + user_list[USER_ID].get_nick_name()));
	}
	find_channel(array[1]).remove_from_channel(user_list[USER_ID]);

	int channel_index = is_channel_active_v2(array[1]);
	if (find_channel(array[1]).get_user_list().size() == 0)
		channel_list.erase(channel_list.begin() + channel_index);
}

void Server::command_quit(int index)
{
	if (user_list[USER_ID].is_verified())
	{
		remove_from_all_channels(user_list[USER_ID], index);
	}
	delete_fd(index);
	delete_user(index);
}

// USER/FD CREATE/DELETE


void Server::create_fd(int fd)
{
	struct pollfd temp;
	temp.fd = fd;
	temp.events = POLLIN;
	temp.revents = 0;

	pfds.push_back(temp);
}

void Server::delete_user(int index)
{
	user_list.erase(user_list.begin() + USER_ID);
}

void Server::delete_fd(int index)
{
	close(pfds[index].fd);
	pfds.erase(pfds.begin() + index);
}

// GETTERS
std::string Server::get_host_name()
{
	return host_name;
}

// SETTER
void Server::set_host_name()
{
	char tmp[255];
	gethostname(tmp, 255);
	host_name = tmp;
}

void Server::set_time()
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	std::time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
	created_time = std::string(buffer);
}

// DEBUG
void Server::print_info() const
{
	int i = 0;
	for (std::vector<User>::const_iterator it = user_list.begin(); it != user_list.end(); ++it)
	{
		std::cout << "User " << i << ": " << (*it).get_nick_name() << ", fd in its class: " << (*it).get_fd() << std::endl;
		++i;
	}
	i = 0;
	for (std::vector<struct pollfd>::const_iterator it = pfds.begin(); it != pfds.end(); ++it)
	{
		std::cout << "Server fd " << i << ": " << (*it).fd << std::endl;
		++i;
	}
}

void Server::print_channel_list() const
{
	int i = 0;
	for (std::vector<Channel>::const_iterator it = channel_list.begin(); it != channel_list.end(); ++it)
	{
		std::cout << "Channel " << i << ": " << (*it).get_name() << std::endl;
		++i;
	}
}