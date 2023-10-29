#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <map>
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include "utils.hpp"
#include <cstdlib>

enum token_type
{
	brackets_open = 0,
	brackets_close, //1
	semicolon, //2
	name, //3
	value, //4
	location, //5
	server, //6
	initialized //7
};

typedef std::list < std::list < std::map<std::string, std::string > > > t_nested_list;
typedef std::list < std::map < std::string, std::string > > t_single_list;

class ConfigParser
{
public:

	~ConfigParser();
/********************************* PUBLIC METHODS *********************************/
	bool										isBroken();				    //ok
	static size_t								numberOfServer();					//ok
	static ConfigParser							&getInstance();						//ok
	static ConfigParser 						&getInstance(std::string fileName); //ok
	static t_nested_list						&getList();                         //ok
	static std::vector<int>						&getPorts();						//ok
	static std::map<std::string, std::string>	&get_globalConfig();				//ok
	static std::map<std::string, std::string> 	&get_defaut_config();				//ok

private:
	std::string							_fileName;			//name of file with config
	std::string							_configFile;		//string which stores raw config line after reading
	token_type							_previousToken;		//type of previous token in config
	t_single_list						_singleList;
	t_nested_list						_serverList;		//this vectore stock all keys word
	bool								_isError;
	std::map<std::string, std::string>	_globalConfig;
	std::vector<int>					_ports;				//this vector going to containt all port of config file
	std::map<std::string, std::string>	_defautConfig;
	std::map<std::string, std::string>	_blockServer;
	std::map<std::string, std::string>	_block_location;
	size_t								_indexConfigFile;
	int									_bracketCounter;
	std::string							_directive_name;
	std::string							_directive_value;
	std::string							_current_word;
	bool								_isLocation;
	std::string							_pwd;

/*************************** CONSTRUCTORS & OVERLOAD ******************************/

	ConfigParser(); // ok
	ConfigParser(std::string fileName); //ok
	ConfigParser(const ConfigParser &other); // ok
	void operator=(ConfigParser &other); // ok

/*************************** INTERFACE AND TOP METHODS ******************************/

	size_t								interface_numberOfServer();     //ok!!!!!!!
	t_nested_list						&interface_getList();		    //ok
	std::vector<int> 					&interface_getPorts();		    //ok
	std::map<std::string, std::string>	&interface_get_globalConfig();  //ok
	std::map<std::string, std::string>	&interface_get_defaut_config(); //ok

	void								parse();  //ok
	void 								setDefaultConfig(); //ok
	bool	 							isNumber(std::string &str);//ok
	void								deleteCommentLine(std::string &line);//ok

	void								checkPort(std::string &str_port);//ok
	void								push_back_in_singleList(std::map<std::string, std::string>	&dictionary);//ok
	void								push_front_in_singleList(std::map<std::string, std::string>	&dictionary);//ok


	/*************************** GETTERS ******************************/
	
	void								openConfFile();					//ok
	int									get_bracketCounter();		//ok
	void								startParsing();			//ok
	bool								get_isLocation();			//ok
	std::string							get_current_word();			//ok
	std::string							get_configFile();			//ok
	t_single_list						get_singleList();			//ok
	std::map<std::string, std::string>	get_defaut_block_serve();	//ok
	std::map<std::string, std::string>	get_blockServer();			//ok
	std::map<std::string, std::string>	get_block_location();		//ok
	std::string							get_current_directory();	//ok	
	std::string							createToken(size_t &i);	//ok
	size_t								get_indexConfigFile();		//ok
	token_type							get_previousToken();		//ok
	std::string							get_directive_name();		//ok
	std::string 						get_directive_value();		//ok

	/*************************** SETTERS ******************************/

	void								set_previousToken(token_type newToken);	//ok
	void								set_directive_name(std::string name);	//ok
	void								set_directive_value(std::string value);	//ok
	void 								set_blockServer(std::string name, std::string value);	//ok
	void								set_block_location(std::map<std::string, std::string> block);	//ok
	void								set_block_location(std::string name, std::string value);	//ok
	void								set_bracketCounter(int value);	//ok
	void								set_current_word(std::string word);	//ok
	void								set_globalConfig(std::string directive_name, std::string directive_value);	//ok
	void								set_blockServer(std::map<std::string, std::string> block_serve);	//ok
	void								push_in_neestedList(t_single_list singleList);	//ok
	void								set_singleList(t_single_list singleList);	//ok
	void								set_isLocation(bool state);	//ok

	/*************************** PARSER ******************************/

 	bool								hasSemicolon(ConfigParser *ptr); 	//ok
 	bool	 							hasValue(ConfigParser *ptr);		//ok
 	bool 								hasDirectName(ConfigParser *ptr);	//ok
 	bool								openBracket(ConfigParser *ptr);//ok
	bool								closeBracket(ConfigParser *ptr);//ok
	void								push_someWhere(ConfigParser *ptr);	//ok
	bool								hasLocation(ConfigParser *ptr);//ok
	void								modify_name_error_page(std::string &directive_name, std::string &directive_value);//ok
	void								not_allow(ConfigParser *ptr);//ok
	bool								blockServer(ConfigParser *ptr);//ok
	void								methodLookup();	//ok
};

#endif