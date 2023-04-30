#include "Server.hpp"

Server::Server(int port) : Socket::Socket(port)
{
	fail_check(bind(fd, (struct sockaddr *)&address, sizeof(address)));
	fail_check(listen(fd, MAX_CLIENT));
	create_fd(fd);
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

void Server::create_user(){
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
