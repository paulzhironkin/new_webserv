#include "WebServer.hpp"

//Constructors

WebServer::WebServer(std::string configFile): _config(ConfigParser::getInstance(configFile))
{
	_compressPollFds = false;
	if (_config.isBroken() == true)
	{
		std::cout<<"Error with config file"<<std::endl;
		exit(EXIT_FAILURE);
	}
	buildServers(); //need to exit with failure inside if some mistake
	printServers();
	launchServers();
}

WebServer::~WebServer() {}







/********************************* METHODS *********************************/

void	WebServer::buildServers()
{
	std::vector<int> &all_ports = ConfigParser::getPorts();

	for (_nfds = 0; _nfds < _config.numberOfServer(); _nfds++)
	{
		Server server; /* instance a server over each iteration*/
		if (all_ports.size() > _nfds)
		{
			server.setPort(all_ports[_nfds]);
		}
		server.setup();
		if (server.isBadSetuped() == true)/*check if a error happened while setup of server it stop the loop*/
			exit(EXIT_FAILURE); /* exit if an error occurred while the setup of server */
		_pollFds[_nfds].fd = server.getServerFD();
		_pollFds[_nfds].events = POLLIN; //Tell me when ready to read
		_listServer.push_back(server);
	}
}

void WebServer::printServers()//НАДО СДЕЛАТЬ ДРУГОЙ ПРИНТ ИНФЫ ПРО СЕРВЕР
{
	std::cout << "\nAvailable servers:" << std::endl << std::endl;
	for (std::list<Server>::iterator it = _listServer.begin(); it != _listServer.end(); it++)
	{
		std::cout << "server =" << it->getServerFD() << "= port =" << it->getPort() << "="  << std::endl;
	}
	std::cout << "\n" << std::endl;
}

void handle_signal(int sig)
{
	std::cout << "Caught signal number = " << sig << std::endl;
}

void WebServer::launchServers()
{
	int ret;
	try
	{
		while (true)
		{
			signal(SIGINT, handle_signal);
			ret = poll(_pollFds, _nfds, -1);
			if (ret == ERROR && errno == EINTR)// check if poll is termined by ctrl-c 
				return ;//ici met throw("exit program");
			//Вопрос тут походу нельзщя еррно использовать
			checkError(ret, "poll() failed");
			if (ret == 0)
				throw std::runtime_error("poll() timed out");
			for (size_t index = 0; index < _nfds; index++) //loop through the array of sokect to monitoring an event 
			{
				//std::cout << "==========_pollFds[index].fd = " << _pollFds[index].fd << " index = " << index << "===========" << std::endl;
				if (_pollFds[index].revents == 0)//loop as long the are not event happened
					continue;
				serveConnectionAt(index);// an event happened, handle it
			}
			clearPoll();
		}
	} catch (const char* error) {
        std::cerr << "Exception: " << error << std::endl;
    }
}

void WebServer::serveConnectionAt(size_t &index)
{
	for (std::list<Server>::iterator it  = _listServer.begin(); it != _listServer.end(); it++)
	{
		Server	&currentServer = *it;
		std::vector<int> &clientSockets = currentServer.getConnectedSockets();
		size_t previousClientsNum = clientSockets.size();
		if (_pollFds[index].fd == currentServer.getServerFD())// meaning new incoming connection
		{
			currentServer.acceptNewConnections();// new socket has been added to "_socketsConnected"
			addFdsToPollFds(clientSockets, previousClientsNum);// poll array should update to by adding the new socket on it
		}
		else if ((std::find(clientSockets.begin(), clientSockets.end(), _pollFds[index].fd)) != clientSockets.end())
		{
			_connection_closed = currentServer.serveEstablishedConnectionFor(&_pollFds[index]); // return true if the connection is closed
			if (_connection_closed == true)
			{
				_compressPollFds = true;// In this case the array  poll will be cleared from fd with -1 condition in poll
			}
		}
	}
}

void WebServer::checkError(int error_code,  const char *  msg)
{
	if (error_code < 0)
		throw std::runtime_error(msg);
}

void WebServer::clearPoll()
{

	if (_compressPollFds)
	{
		_compressPollFds = false;
		for (size_t i = 0; i < _nfds; i++)
		{
			if (_pollFds[i].fd == -1)
			{
				for(size_t j = i; j < _nfds - 1; j++)
				{
					_pollFds[j].fd = _pollFds[j+1].fd;
				}
				i--;
				_nfds--;
			}
		}
	}
}

void WebServer::addFdsToPollFds(std::vector<int> &clientSockets, size_t previousClientsNum)
{
	for (size_t i = previousClientsNum; i < clientSockets.size(); i++)
	{
		_pollFds[_nfds].fd = clientSockets[i];
		_pollFds[_nfds].events = POLLIN;//Tell me when ready to read
		_pollFds[_nfds].revents = 0;//set to 0 - none event, like that if value  changed it mean event a happened
		_nfds++;
	}
}