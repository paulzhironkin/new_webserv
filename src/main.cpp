#include <iostream>
#include "WebServer.hpp"

int main(int argc, char *argv[])
{
	if (argc > 2)
		std::cout<<"Invalid number of arguments"<<std::endl;
	else if (argc == 2)
	{
		WebServer webserv(argv[1]);
	}
	else
	{
		WebServer webserv("./configs/default.conf");
	}
	return 0;
}