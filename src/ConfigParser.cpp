#include "ConfigParser.hpp"

typedef bool (ConfigParser::*method_function)(ConfigParser *);

static std::string s_fileName;

static bool	is_key_word(std::string &word)
{
	if (word.compare("listen") == 0)
		return (true);
	if (word.compare("server_name") == 0)
		return (true);
	if (word.compare("root") == 0)
		return (true);
	if (word.compare("error_page") == 0)
		return (true);
	if (word.compare("autoindex") == 0)
		return (true);
	if (word.compare("client_max_body_size") == 0)
		return (true);
	if (word.compare("index") == 0)
		return (true);
	if (word.compare("allow_methods") == 0)
		return (true);
	if (word.compare("allow") == 0)
		return (true);
	if (word.compare("cgi") == 0)
		return (true);
	if (word.compare("fastcgi_pass") == 0)
		return (true);
	if (word.compare("fastcgi_param") == 0)
		return (true);
	if (word.compare("return") == 0)
		return (true);
	if (word.compare("cli_max_size") == 0)
		return (true);
	if (word.compare("cgi_pass") == 0)
		return (true);
	return (false);
}


/********************************* PUBLIC METHODS *********************************/

size_t	ConfigParser::numberOfServer() {return(getInstance(s_fileName).interface_numberOfServer());}

bool ConfigParser::isBroken() {return(_isError);}

t_nested_list &ConfigParser:: getList() {return(getInstance(s_fileName).interface_getList());}

std::vector<int> &ConfigParser::getPorts() {return(getInstance(s_fileName).interface_getPorts());}

std::map<std::string,std::string> &ConfigParser::get_globalConfig() {return(getInstance(s_fileName).interface_get_globalConfig());}

std::map<std::string,std::string> &ConfigParser::get_defaut_config(){return(getInstance(s_fileName).interface_get_defaut_config());}

ConfigParser &ConfigParser::getInstance(std::string fileName)
{
	static  ConfigParser  instance(fileName);
	return (instance);
}

/*************************** CONSTRUCTORS & OVERLOAD ******************************/

ConfigParser::ConfigParser(): 
	_fileName("./configs/default.conf"), _configFile(""), _isError(false)
{
	s_fileName = "./configs/default.conf";
	std::cout<<"**************** DEFAULT CONFIG FILE ********************\n";
	startParsing();
}

ConfigParser::ConfigParser(std::string fileName): 
		_fileName(fileName), _configFile(""), _isError(false)
{
	s_fileName = fileName;
	std::cout<<"**************** SELECTED CONFIG FILE ********************\n";
	startParsing();
}

ConfigParser::ConfigParser(const ConfigParser &other) {(void)other;};

void ConfigParser::operator=(ConfigParser &other) {(void)other;}

ConfigParser::~ConfigParser(){}


/*************************** INTERFACE AND TOP METHODS ******************************/

size_t	ConfigParser::interface_numberOfServer() {return (_serverList.size());}
t_nested_list	&ConfigParser::interface_getList() {return(_serverList);}
std::vector<int> &ConfigParser::interface_getPorts() {return(_ports);}
std::map<std::string, std::string> &ConfigParser::interface_get_globalConfig() {return(_globalConfig);}
std::map<std::string, std::string> &ConfigParser::interface_get_defaut_config() {return(_defautConfig);}

void ConfigParser::parse()
{
	for (; _indexConfigFile < _configFile.size();)
	{
		if (!isspace(_configFile[_indexConfigFile]))
		{
			_current_word = createToken(_indexConfigFile);
			methodLookup();
		}
		else
		{
 			_indexConfigFile++;
		}
	}
}

void ConfigParser::setDefaultConfig()
{
	_defautConfig["index"]			= "index.html";
	_defautConfig["allow"]			= "GET POST DELETE";
	_defautConfig["listen"]			= "8080";
	_defautConfig["cgi_pass"]		= "/usr/bin/php-cgi";
	_defautConfig["autoindex"]		= "on";
	_defautConfig["error page"]		= "404 error.html";
	_defautConfig["server_name"]	= "latahbah.com";
	_defautConfig["cli_max_size"]	= "5000000";
	char *pwd = getcwd(NULL, 0);
	std::cout<<"getcwd return :["<<pwd<<"]\n";
	if (pwd)
	{
		_pwd = pwd;
		_defautConfig["root"] = pwd;
		_defautConfig["root"] += "/www";
		free(pwd);
	}
}

bool ConfigParser::isNumber(std::string &str)
{
	int result;
	for (size_t i = 0; i < str.size(); i++)
	{
		result = isdigit(str[i]);
		if (result == 0)
		{
			return (false);
		}
	}
	return (true);
}

void ConfigParser::deleteCommentLine(std::string &line)
{
	size_t  begin = line.find("#");
	if (begin != std::string::npos)
	{
		line.erase(begin);
	}
}

void ConfigParser::push_back_in_singleList(std::map<std::string,std::string> &dictionary)
{
	if (dictionary.size())
		_singleList.push_back(dictionary);
}

void ConfigParser::push_front_in_singleList(std::map<std::string, std::string>	&dictionary)
{
	if (dictionary.size())
		_singleList.push_front(dictionary);
}

void ConfigParser::checkPort(std::string &str_port)
{
	int int_port ;
	bool ret = isNumber(str_port);
	if (ret == false)
		throw std::runtime_error("port error");
	std::stringstream(str_port) >> int_port; // convert string to integer
	_ports.push_back(int_port);
}



/*************************** GETTERS ******************************/

bool ConfigParser::get_isLocation() {return(_isLocation);}
std::string	ConfigParser::get_configFile() {return(_configFile);}
std::string	ConfigParser::get_current_directory() {return(_pwd);}
t_single_list ConfigParser::get_singleList() {return(_singleList);}
int	ConfigParser::get_bracketCounter() {return(_bracketCounter);}
std::string	ConfigParser::get_current_word() {return(_current_word);}
std::map<std::string,std::string> ConfigParser::get_defaut_block_serve() {return(_defautConfig);}
std::map<std::string, std::string> ConfigParser::get_blockServer() {return(_blockServer);}
std::map<std::string, std::string> ConfigParser::get_block_location() {return(_block_location);}
size_t ConfigParser::get_indexConfigFile() {return(_indexConfigFile);}
token_type ConfigParser::get_previousToken() {return(_previousToken);}
std::string ConfigParser::get_directive_name() {return(_directive_name);}
std::string ConfigParser::get_directive_value() {return(_directive_value);}


void ConfigParser::openConfFile()
{
	std::string line;
	std::ifstream inputFile(_fileName.c_str());
	if (inputFile.is_open())
	{
		while (getline(inputFile, line)) 
		{
			_configFile.append(line);
			deleteCommentLine(_configFile);
			_configFile.append("\n");
		}
		inputFile.close();
	}
	else
	{
		throw std::runtime_error("Open file error");
	}
}

void ConfigParser::startParsing()
{
	try {
		openConfFile();
		_bracketCounter = 0;
		_previousToken = initialized;
		_indexConfigFile = 0;
		_isLocation = false;
		setDefaultConfig();
		_blockServer = _defautConfig;
		parse();
		std::cout << "Config file - OK" << std::endl; 
	} catch (const std::exception& e) {
		_isError = true;
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

std::string	ConfigParser::createToken(size_t &i)
{
	size_t	numCharToCopy = 0;
	size_t	start = i;

	if (_configFile[i] == '{' || _configFile[i] == '}' || _configFile[i] == ';')//if { or } or ; is the first character i going to catch that piece Of String
	{
		i++;
		numCharToCopy++;
	}
	else
	{
		while (!isspace(_configFile[i]) && (_configFile[i] != '{' && _configFile[i] != '}') && _configFile[i] != ';') // { or } or ; and whitespace character are delimiter
		{
			numCharToCopy++;
			i++;
		}
	}
	std::string new_token = _configFile.substr(start, numCharToCopy);
	return (new_token);
}

/*************************** SETTERS ******************************/

void ConfigParser::set_previousToken(token_type newToken) {_previousToken = newToken;}
void ConfigParser::set_directive_name(std::string name) {_directive_name = name;}
void ConfigParser::set_directive_value(std::string value) {_directive_value = value;}
void ConfigParser::set_blockServer(std::string name, std::string value) {_blockServer[name] = value;}
void ConfigParser::set_blockServer(std::map<std::string, std::string> block_serve) {_blockServer = block_serve;}
void ConfigParser::set_block_location(std::map<std::string, std::string> block) {_block_location = block ;}
void ConfigParser::set_block_location(std::string name, std::string value) {_block_location[name] = value;}
void ConfigParser::set_bracketCounter(int value) {_bracketCounter = value;}
void ConfigParser::set_current_word(std::string word) {_current_word = word;}
void ConfigParser::set_singleList(t_single_list singleList) {_singleList = singleList;}
void ConfigParser::set_isLocation(bool state) {_isLocation = state;}
void ConfigParser::set_globalConfig(std::string directive_name, std::string directive_value)
{
	int bracket_counter = get_bracketCounter();
	if ( bracket_counter == 0)
	{
		_globalConfig[directive_name] = directive_value;
	}
}
void ConfigParser::push_in_neestedList(t_single_list singleList)
{
	_serverList.push_back(singleList);
}

/*************************** PARSER ******************************/

bool ConfigParser::hasSemicolon(ConfigParser *ptr)
{
	std::string current_word = ptr->get_current_word();
	if (current_word.compare(";") == 0)
	{
		token_type previousToken = ptr->get_previousToken();
		if (previousToken == value)
		{
			not_allow(ptr);/*check authorization on some directive name or directive value*/
			ptr->set_previousToken(semicolon);
			if (ptr->get_bracketCounter() == 0)/*were outside block server the data will be store in global config*/
				ptr->set_globalConfig(ptr->get_directive_name(), ptr->get_directive_value());/*go head to store data in global config*/
			else if (ptr->get_isLocation() == true)/* if where are in block location*/
				ptr->set_block_location(ptr->get_directive_name(), ptr->get_directive_value());/*go head to store data block location*/
			else
				ptr->set_blockServer(ptr->get_directive_name(), ptr->get_directive_value());/*we are in server block   the data will store there*/
			return (true);
		}
		else
			throw std::runtime_error("Syntax error: hasSemicolon func");
	}
	return (false);
}

bool ConfigParser::hasValue(ConfigParser *ptr)
{
	token_type previousToken = ptr->get_previousToken();
	if (previousToken == name || previousToken == location || previousToken == value)
	{
		std::string current_word = ptr->get_current_word();
		std::string directiveValue = ptr->get_directive_value();
		if (previousToken == value)
			directiveValue += " " + current_word;
		else
			directiveValue = current_word;
		ptr->set_directive_value(directiveValue);
		ptr->set_previousToken(value);
		return (true);
	}
	else
	{
		std::cout << "ptr->get_current_word() == [" << ptr->get_current_word() << "]" << std::endl;
		throw std::runtime_error("Syntax error: hasValue func");
	}
	return (false);
}

bool ConfigParser::hasDirectName(ConfigParser *ptr)
{
	std::string current_word = ptr->get_current_word();
	if (is_key_word(current_word) == true)
	{
		std::string configFile = ptr->get_configFile();
		int result = !isspace(configFile[ptr->get_indexConfigFile()]);/**/
		if (result != 0)/*after a name i should have a space*/
		{
			throw std::runtime_error("Syntax error: directory name with space");
		}
		token_type previousToken = ptr->get_previousToken();
		std::cout<<"prev token: "<<previousToken<<" is cur_word: "<<current_word<<std::endl;

		if (previousToken == semicolon || previousToken == brackets_open || previousToken == brackets_close || previousToken == initialized)//the rule of layout of config file
		{
			ptr->set_directive_name(current_word);
			ptr->set_previousToken(name);
			return (true);
		}
		else
			throw std::runtime_error("Directive name error");
	}
	return (false);
}

bool ConfigParser::openBracket(ConfigParser *ptr)
{
	std::string current_word = ptr->get_current_word();
	if (current_word.compare("{") == 0)
	{
		token_type previousToken = ptr->get_previousToken();
		if (previousToken == server || previousToken == location || previousToken == value)
		{
			int n = ptr->get_bracketCounter();
			n++;
			ptr->set_bracketCounter(n);
			previousToken = brackets_open;
			if (ptr->get_isLocation() == true)
				ptr->set_block_location(ptr->get_directive_name(), ptr->get_directive_value());
			ptr->set_previousToken(previousToken);
			return (true);
		}
		else
		{
			throw std::runtime_error("Syntax error: unexpected open bracket");
		}
	}
	return (false);
}

bool ConfigParser::closeBracket(ConfigParser *ptr)
{
	std::string word = ptr->get_current_word();
	if (word.compare("}") == 0)
	{
		token_type previousToken = ptr->get_previousToken();
		if ( previousToken == semicolon || previousToken == brackets_close || previousToken == brackets_open)
		{
			int n = ptr->get_bracketCounter();
			ptr->set_bracketCounter(--n);
			push_someWhere(ptr);
			previousToken = brackets_close;
			ptr->set_previousToken(previousToken);
			return (true);
		}
		else
		{
			throw std::runtime_error("Syntax error: unexpected close bracket");
		}
	}
	return (false);
}

static bool	there_error_page(std::map < std::string, std::string > &dictionary, std::string &path_error_page, std::string &root_location)
{
	for (std::map < std::string, std::string >::iterator it = dictionary.begin(); it != dictionary.end() ; it++)
	{
		int cmp = it->first.compare(0, 10, "error_page");
		if (cmp == 0)
		{
			/*a error page has been find*/
			path_error_page = it->second;
			if (dictionary.find("root") != dictionary.end())/*check if the are a path root */
				root_location = dictionary["root"];
			// std::cout << "root_location == " << root_location << std::endl;
			return (true);
		}
	}
	return (false);
}

void verrify_error_page(t_single_list &secondList)
{
	bool ret = true;
	std::map < std::string, std::string > dictionary;
	std::map < std::string, std::string >::iterator it;
	t_single_list::iterator itr_secondList = secondList.begin();
	dictionary = *itr_secondList;
	std::string path_error_page;
	std::string root_location;
	std::string server_root = dictionary["root"]; 
	int i = 0;
	for (;  itr_secondList != secondList.end(); itr_secondList++)
	{
		i++;
		if (there_error_page(*itr_secondList, path_error_page, root_location) == true)
		{
			/*if a error page has been find path_error_page will contain the path same to root_location*/
			if (i == 0)
			{
				// std::cout << "block server"<< std::endl;
				// ret = verrify_path_error_page(dictionary["error_page"], dictionary["root"]);
				ret = exists( dictionary["root"]+ "/" + dictionary["error_page"]);/* this function return true or false according to the existence of file*/
			}
			else
			{
				if (root_location.size() == 0)
				{
					/*the block location doesn't have a root inside the dictionary the why it take the root of block server*/
					root_location = server_root;
				}
				if (dictionary.find("root") != dictionary.end())
				{
					// std::cout << "block location with root"<< std::endl;
					// ret = verrify_path_error_page(path_error_page, root_location);
					ret = exists(root_location + "/" + path_error_page);/* this function return true or false according to the existence of file*/
				}
				else
				{
					// std::cout << "block location without root"<< std::endl;
					// ret = verrify_path_error_page(path_error_page, server_root);
					ret = exists(server_root + "/" + path_error_page);/* this function return true or false according to the existence of file*/
				}
			}
		}
		if (ret == false)
			throw("error: path of 'error page' doesn't existed");
	}
}

void ConfigParser::push_someWhere(ConfigParser *ptr)
{
	std::map <std::string, std::string> blockServer = ptr->get_blockServer();
	if (ptr->get_bracketCounter() == 0)/*out side of any block */
	{
		ptr->push_front_in_singleList(blockServer);
		t_single_list singList = ptr->get_singleList();
		ptr->push_in_neestedList(singList);
		verrify_error_page(singList);
		ptr->set_singleList(t_single_list());
		std::map <std::string, std::string> defaut_config = ptr->get_defaut_block_serve();
		ptr->set_blockServer(defaut_config);
	}
	else if (ptr->get_isLocation() == true)
	{
		std::map <std::string, std::string> block_location = ptr->get_block_location();
		ptr->push_back_in_singleList(block_location);
		ptr->set_block_location(std::map <std::string, std::string>());
		ptr->set_isLocation(false);
	}
}

bool ConfigParser::hasLocation(ConfigParser *ptr)
{
	std::string current_word = ptr->get_current_word();
	if (current_word.compare("location") == 0)
	{
		token_type	previousToken = ptr->get_previousToken();
		if ((previousToken == semicolon || previousToken == brackets_open || previousToken == brackets_close) && ptr->get_isLocation() == false)
		{
			ptr->set_directive_name(current_word);
			ptr->set_previousToken(location);
			ptr->set_isLocation(true);
			return (true);
		}
		else
		{
			throw std::runtime_error("Syntax error: hasLocation func");
		}
	}
	return (false);
}

void ConfigParser:: modify_name_error_page(std::string &directive_name, std::string &directive_value)
{
	size_t pos = directive_value.find(" ");
	std::string name = "error_page ";
	name += directive_value.substr(0, pos);
	std::string value = directive_value.substr(pos + 1);
	directive_name = name;
	directive_value = value;
}

void ConfigParser::not_allow(ConfigParser *ptr)
{
	std::string directive_name = ptr->get_directive_name();
	std::string directive_value = ptr->get_directive_value();
	std::string current_directory = ptr->get_current_directory();
	if (directive_name.compare("return") == 0 && ptr->get_isLocation() == false)
		throw std::runtime_error("return outside of location block error");
	if (directive_name.compare("root") == 0)
	{
		std::cout << "current_directory == [" << current_directory << "]  received == [" << directive_value << "]" << std::endl;
		bool ret = is_a_directory(directive_value);
		if (ret == false)
		{
			throw std::runtime_error("path error");
		}
	}
	if (directive_name.compare("listen") == 0 )
		ptr->checkPort(directive_value);
	if (directive_name.compare("listen") == 0 && ptr->get_isLocation() == true)
		throw std::runtime_error("listen cmd error");
	if (directive_name.compare("autoindex") == 0 && directive_value.compare("off") != 0 && directive_value.compare("on") != 0)
		throw std::runtime_error("autoindex status error");
	if (directive_name.compare("error_page") == 0 )
	{
		if (error_page_syntax(directive_value) == false) //сделать throws из функции подумать над этим
			throw std::runtime_error("error page error");
		else
		{
			modify_name_error_page(directive_name, directive_value);
			ptr->set_directive_name(directive_name);
			ptr->set_directive_value(directive_value);
		}

	}
}

bool ConfigParser::blockServer(ConfigParser *ptr)
{
	std::string current_word = ptr->get_current_word();
	if (current_word.compare("server") == 0)
	{	
		token_type previousToken = ptr->get_previousToken();
		if (previousToken == initialized || previousToken == brackets_close || previousToken == semicolon)
		{
			previousToken = server;
			ptr->set_previousToken(previousToken);
			return (true);
		}
		else
		{
			std::cout << "previousToken == " << previousToken << std::endl;
			throw std::runtime_error("server error");
		}
	}
	return (false);
}

void ConfigParser::methodLookup()
{
	bool returnMethod = false;
	/*all methode in this array let to done parse  each token = @_current_word, to knowing the rules of parse go take a look the diagram */
	method_function methods[6]	=  {&ConfigParser::blockServer, 
									&ConfigParser::openBracket, 
									&ConfigParser::closeBracket, 
									&ConfigParser::hasDirectName, 
									&ConfigParser::hasSemicolon, 
									&ConfigParser::hasLocation };
	for (size_t i = 0; i < 6; i++)
	{
		/* this loop  look  up the methode match up with the @_current_word then apply the rule parsing*/
		method_function curMethod = methods[i];
		returnMethod = (this->*curMethod)(this);
		if (returnMethod == true)
			break;/*this instruction let to stop the loop it meant the @_current_word has been identify*/
	}
	/*if nothing has been find this instruction will be execute by default*/
	if (returnMethod == false)
		hasValue(this);
}