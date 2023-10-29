#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <poll.h>
#include <cstddef>
#include <string>
#include <list>
#include "Server.hpp"
#include "ConfigParser.hpp"
#include <iostream>
#define BUFFER_SIZE 10000
#define POLLFD_SIZE 1000

class WebServer
{
public:
	WebServer(std::string configFile);
	~WebServer();
	void				launchWebServer();

private:
	char 				_buf[BUFFER_SIZE];
	struct pollfd		_pollFds[POLLFD_SIZE];
	size_t				_nfds;
	ConfigParser		&_config; 
	bool				_connection_closed;
	bool				_compressPollFds;
	std::list<Server> 	_listServer;

	void				buildServers();
	void				printServers();
	void				launchServers();

	void				serveConnectionAt(size_t &index); //mod from here
	void				checkError(int error_code,  const char *  msg);
	void				clearPoll();
	void				addFdsToPollFds(std::vector<int> &vect_socket_fd, size_t from);

};

#endif