#ifndef CGI_HPP
#define CGI_HPP
# ifndef CGI_PATH
#  define CGI_PATH "/usr/bin/php-cgi"
# endif

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "utils.hpp"
#include "Request.hpp"

#define ERROR -1
#define SUCCESS 0
#define TO_READ 0
#define TO_WRITE 1

class Cgi
{
public:
	Cgi(std::string script, void *ptr_void, std::map<std::string, std::string> &reponse);//ok
	std::string			get_data();//ok
	~Cgi();//ok

private:
	Cgi();//ok
	std::string							_script;
	std::string 						_cgi_path;
	char								**_env;
	char								**_args;
	std::string							_data;
	std::string							_pwd;
	std::string							_uri;
	bool								_has_error;
	std::map<std::string, std::string> 	_env_map;
	std::string							_cgi_body;



	void			complete_the_name_of_script();//ok
	void			set_args(); //ok
	void			set_env(); //ok
	void			check_error(int code, const char *error_msg);//ok
	void			clear_args(); //ok
	void			exec_Cgi(); //ok
	void			clear_2D_array(char **array);//ok
	void			remove_headers(std::map<std::string, std::string> &cgi_head);//ok
	void			set_env_map(void *ptr_void);//ok
};

#endif