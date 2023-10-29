#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <cstring> 
#include <exception>
#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "Request.hpp"


#define SERV_BUF_SIZE 10000
#define NB_PENDING 30000
#define ERROR -1
class Server
{
public:
	Server(); //ok
	Server(int port); //ok
	~Server(); //ok

	void					setup(); //ok
	void					acceptNewConnections(); //ok
	bool					serveEstablishedConnectionFor(struct pollfd	*_tab_poll);//ok
	void					setPort(int port);//ok
	int						getPort();//ok
	int	 					isBadSetuped();//ok
	int						getServerFD();//ok
	std::vector<int>		&getConnectedSockets(); //ok

private:
	int						_serverFD;
	int						_listen_fd;
	char					_buffer[SERV_BUF_SIZE + 1];
	int						_port;
	bool					_closedConnection;
	bool					_isServerError;
	std::vector<int>		 _socketsConnected;//this vector will store  socket clients which reached server 
	struct	sockaddr_in		_address;

	void					testError(int error_code, const char  * msg); //ok
	int						receiveData(struct pollfd	*_tab_poll); //ok
	void					eraseClientSocketFD(int to_find); //ok
};

#endif