#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "utils.hpp"
#include "Cgi.hpp"

#define SUCCESS 0
#define FAILURE 1
#define REQ_BUF_SIZE 10000

struct my_equal {
    bool operator()(char ch1, char ch2) {
        return std::toupper(ch1) == std::toupper(ch2);
    }
};

class Request
{
public:
	Request();
	~Request();

	std::map<std::string, std::string>	header;
	std::map<std::string, std::string>	reponse;
	std::map<std::string, std::string>	cgi_head;
	std::vector<char>					request;

	int						store(char buffer[REQ_BUF_SIZE], struct pollfd *ptr_tab_poll, int bytes);//ok
	void					parseText(struct pollfd *ptr_tab_poll, int port);  //ok
	void                    processRequest(); //ok
	void					composeReponse(struct pollfd *ptr_tab_poll); //ok
	int						sendResponse(struct pollfd *ptr_tab_poll);	//ok
	bool					isFullyReceived(struct pollfd *ptr_tab_poll); //ok
	int                     create_file(std::string const file_type); //ok
	std::string				find_url_and_name_from_file(std::string const file_type);  //ok
	std::string				return_config_info(std::string searching_index); //ok

	std::string				get_method();			//ok
	std::string				get_body();				//ok
	std::string				get_path();				//ok
	std::string				get_content_type();		//ok
	std::string				get_content_length();	//ok
	std::string				get_path_info();		//ok - unused method!!!!
	std::string				get_host(); 			//ok
	std::string				get_accept(); 			//ok
	std::string				get_accept_encoding();	//ok
	std::string				get_accept_language();	//ok

private:
	char 								buf[REQ_BUF_SIZE + 1];
	std::map<std::string, std::string>  http_table();   	 		//ok
	void                                _process_GET();				//ok
    void                                _process_POST();			//ok
    void                                _process_DELETE();			//ok
    void                                http_code(std::string code);//ok
	std::map<std::string, std::string>	initialize_mime_types();  	//ok

	
};

#endif