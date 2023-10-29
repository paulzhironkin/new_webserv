#include "Cgi.hpp"

Cgi::Cgi() {}
Cgi::~Cgi() {}
Cgi::Cgi(std::string script, void *ptr_void, std::map<std::string, std::string> &cgi_head):
	_script(script), _cgi_path(CGI_PATH), _env(NULL), _args( NULL)
{
	_data = std::string();
	_has_error = false;
	try
	{
		complete_the_name_of_script();
		set_args();
		set_env_map(ptr_void);
		set_env();
		exec_Cgi();
		remove_headers(cgi_head);
	} catch (const char* error) {
        std::cerr << "Exception: " << error << std::endl;
    }
	clear_2D_array(_env);
	clear_2D_array(_args);
}

std::string	Cgi::get_data() {return(_data);}

void Cgi::complete_the_name_of_script()
{
	char *pwd = NULL;

	_script = "/" + _script;
	std::string tmp = _script;
	pwd = getcwd(NULL, 0);
	_pwd = pwd;//stock pwd 
	if (pwd == NULL)
	{
		throw("error while getcwd");
		_has_error = true;
	}
	else
	{
		_script = pwd;
		if (_script.find("/www") != std::string::npos)
		{
			_script += tmp;
		}
		else
		{
			_script += "/www" + tmp;
		}
		free(pwd);
	}
}

void Cgi::set_args()
{
	 _args = (char**)malloc(sizeof(char *) * 3); //0 - path, 1 - name, 2 - NULL
	char *name = strdup((char*)_script.c_str());
	char *path =  strdup((char*)_cgi_path.c_str());
	if (name == NULL)
		throw("error alloc for name");
	if (path == NULL)
		throw("error mal for path");
	_args[0] = path;
	_args[1] = name;
	_args[2] = NULL;
}
void Cgi::set_env()
{
	_env = (char **)malloc(sizeof(char *) * (_env_map.size() + 1));
	if (_env == NULL)
		throw("error happened while mallo in set_env() to cgi");
	int i = 0;
	for(std::map<std::string, std::string>::iterator it = _env_map.begin(); it != _env_map.end(); it++)
	{
		_env[i] = strdup((it->first + "=" + it->second).c_str());
		if (_env[i] == NULL)
			throw("error happened while mallo in set_env() to cgi");
		i++;
	}
	_env[i] = NULL;
}

void Cgi::check_error(int code, const char *error_msg)
{
	if (code < 0)
		throw std::runtime_error(error_msg);
}

void Cgi::clear_args()
{
	for (size_t i = 0; i < 2 ; i++)
	{
		free(_args[i]);
		_args[i] = NULL;
	}
	free(_args);
	_args = NULL;
}

void Cgi::exec_Cgi()
{
 	int			pipeFd[2];
	int			child_status;

	pipe(pipeFd);
	int			pid = fork();
	check_error(pid, "error cgi fork failed\n");

	if (pid == 0)
	{
		close(pipeFd[TO_READ]);/*closing read side of pipe because we're only going to write*/
		dup2(pipeFd[TO_WRITE], 1);  /* connect the write side with stdout */

		if (_cgi_body.length() > 0) {
			int pipe2[2];
			pipe(pipe2);

			dup2(pipe2[TO_READ], 0);

			write(pipe2[TO_WRITE], _cgi_body.c_str(), _cgi_body.size());
			close(pipe2[TO_WRITE]); // send EOF
		}

		if (execve(_args[0], _args, _env) == ERROR) {
			exit(ERROR);
		}
		exit(SUCCESS);
	}
	else
	{
		close(pipeFd[TO_WRITE]);/*closing of write side of pipe because it read*/
		int ret = wait(&child_status);
		check_error(ret, "error wait");
		check_error(child_status, "error execve");
		char		c; /* this variable will skim to each character in pipeFd[TO_READ] */
		while (read(pipeFd[TO_READ], &c, 1) > 0)/*from, here contains in pipeFd[TO_READ] gonna be copy in string "_data"*/
		{
			_data += c; 
		}
	}
}

void Cgi::clear_2D_array(char **array)
{
	int i = 0;
	while (array[i])
	{
		free(array[i]);
		i++;
	}
	free(array);
}

void Cgi::remove_headers(std::map<std::string, std::string> &cgi_head) {

	std::string	header("");
	std::string key;
	std::string value;

	size_t pos2;
	size_t pos = _data.find("\r\n\r\n");
	if (pos != std::string::npos) {

		header = _data.substr(0, pos + 2);
		_data = _data.substr(pos + 4, std::string::npos);
	
	}
	while ((pos = header.find("\r\n")) != std::string::npos
			&& (pos2 = header.find(":")) != std::string::npos
 		 	&& pos2 < pos) {

		key = header.substr(0, pos2);
		value = header.substr(pos2 + 2, pos - pos2 - 2);

		cgi_head[key] = value;

		header.erase(0, pos + 2);
	}
}

void Cgi::set_env_map(void *ptr_void)
{
	/*
		Set cgi body to empty string to check when we execve to know if there is something to send in STDIN
	*/
	_cgi_body = "";

	Request *ptr_request = (Request *)ptr_void;
	_env_map["AUTH_TYPE"] = "";
	_env_map["CONTENT_LENGTH"] = "0";
	_env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env_map["PATH_TRANSLATED"] = "";//_args[0];
	_env_map["REDIRECT_STATUS"] = "200";
	_env_map["REQUEST_METHOD"] = ptr_request->header["method"];
	_env_map["SCRIPT_FILENAME"] = _args[1];
	_env_map["SCRIPT_PORT"] = ptr_request->header["port"];
	_env_map["SERVER_NAME"] = "webserv";
	_env_map["SERVER_PROTOCOL"] = "HTTP/1.1";

	if (ptr_request->header["method"] == "GET" && ptr_request->header["args"] != "")
	{
		_env_map["QUERY_STRING"] = ptr_request->header["args"];
		_cgi_body = ptr_request->header["args"];
	}
	if (ptr_request->header["method"] == "POST")
	{
		_env_map["CONTENT_LENGTH"] = ptr_request->header["CONTENT-LENGTH"];
		_env_map["CONTENT_TYPE"] = ptr_request->header["CONTENT-TYPE"];
		_cgi_body = ptr_request->header["body"];
	}
}