#include "Server.hpp"

Server::Server(): _port(8080), _isServerError(false) {}
Server::Server(int port): _port(port), _isServerError(false) {}
Server::~Server(){}

void Server::setup()
{
	int ret;
	int	option = 1;
	try
	{
		_serverFD = socket(AF_INET, SOCK_STREAM, 0);//  Creating socket file descriptor, SOCK_STREAM: TCP(reliable, connection oriented)
		testError(_serverFD, "error create socket");
		ret = setsockopt(_serverFD, SOL_SOCKET,  SO_REUSEADDR, (char *)&option, sizeof(option));// Forcefully attaching socket to the port
		testError(ret, "bfrforce to reuse a port function setsockopt");
		ret = fcntl(_serverFD, F_SETFL, O_NONBLOCK);// Set socket to be nonblocking
		testError(ret, "Error function fcnt");
		_address.sin_family = AF_INET;// IPv4 protocol
		memset(&_address.sin_zero, 0, sizeof (_address.sin_zero));
		_address.sin_addr.s_addr = INADDR_ANY;
		_address.sin_port = htons(_port);
		ret = bind(_serverFD, (struct sockaddr *)&_address, sizeof(_address));// Forcefully attaching socket to the port
		testError(ret, "Error while the binding");
		_listen_fd = listen(_serverFD, NB_PENDING);
		testError(_listen_fd, "Error function  the listening");
	} catch (const std::exception& e) {
		_isServerError = true;
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void Server::acceptNewConnections()
{
	int new_socket = 0;
	int addrlen = sizeof(_address);
	while (new_socket != ERROR)
	{
		new_socket = accept(_serverFD, (sockaddr *)&_address, (socklen_t*)&addrlen);
		if (new_socket < 0)
		{
 			if (errno != EWOULDBLOCK)
				testError(ERROR, "accept() failed");
			new_socket = ERROR;
		}
		else
		{
			_socketsConnected.push_back(new_socket);//add new socket in vector socket, it has all  client of own port
		}
	}
}

bool Server::serveEstablishedConnectionFor(struct pollfd *ptr_tab_poll)
{
	Request		request;
	int			ret;

	_closedConnection = false;
	if (ptr_tab_poll->revents & POLLOUT) {

		ret = request.sendResponse(ptr_tab_poll);

		if (ret < 0)
			std::cout << "send() failed!" << std::endl;
		else if (ret == 0) {
			close(ptr_tab_poll->fd);
			_closedConnection = true;
		}

	}
	else if ((ret = receiveData(ptr_tab_poll)) > 0)
	{
		request.store(_buffer, ptr_tab_poll, ret);
		if (request.isFullyReceived(ptr_tab_poll))
		{
			request.parseText(ptr_tab_poll, _port);
			request.processRequest();
			request.composeReponse(ptr_tab_poll);
		}
	}
	if (_closedConnection)
	{
		std::cout << "close connection pollFd  = " << ptr_tab_poll->fd << "\n" <<std::endl;
		close(ptr_tab_poll->fd);
		eraseClientSocketFD(ptr_tab_poll->fd);
		ptr_tab_poll->fd = -1;
	}
	return (_closedConnection);
}

int	 Server::getPort() {return(_port);}
void Server::setPort(int new_port) {_port = new_port;}
int	 Server::isBadSetuped() {return(_isServerError);}
int	 Server::getServerFD() {return(_serverFD);}
std::vector<int> &Server::getConnectedSockets() {return(_socketsConnected);}

void Server::testError(int error_code,  const char *  msg)
{
	if (error_code < 0)
	{
		close(_serverFD);
		throw std::runtime_error(msg);
	}
}

int Server::receiveData(struct pollfd	*ptr_tab_poll)
{	
	int ret = recv(ptr_tab_poll->fd, _buffer, SERV_BUF_SIZE, 0);
	if (ret < 0)
	{
		_closedConnection = true;
		std::cout << "recv() failed" << std::endl;
		return (ret);
	}
	if (ret == 0)
	{
		std::cout << "connection closed from remote side" << std::endl;
		_closedConnection = true;
		return(ret);
	}
	_buffer[ret] = '\0';

	std::cout << "\n\n" << "===============   "  << ret << " BYTES  RECEIVED   ===============\n";
	std::cout << _buffer;
	std::cout << "\n======================================================" << std::endl;

	return (ret);	
}

void Server::eraseClientSocketFD(int to_find)
{
	for (std::vector<int>::iterator it = _socketsConnected.begin(); it !=  _socketsConnected.end() ; it++)
	{
		if (*it == to_find)
		{
			_socketsConnected.erase(it);
			return ;
		}
	}
}