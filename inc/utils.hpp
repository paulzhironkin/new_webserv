#ifndef UTILS_HPP
#define UTILS_HPP

#include <map>
#include <list>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <cstring>
#include "ConfigParser.hpp"

typedef std::list < std::list < std::map < std::string, std::string > > > t_nested_list;
typedef std::list<std::map < std::string, std::string > > t_single_list;

bool	find_location(t_single_list & secondList, std::string &elemToFind, void *ptrReponse);
bool	isLocation(std::map < std::string, std::string > &dictionary);
bool	find_directive(t_single_list & secondList, std::string &elemToFind, void *ptrReponse);
bool	getInfo(int port, std::string elemToFind, void *reponse, bool(*callback)(t_single_list & secondList, std::string &elemToFind, void *ptrReponse));
bool	get_location_url(int port, std::string url, void *ptrReponse);
bool	is_a_directory(const std::string &s);
bool	exists(const std::string &s);
bool	error_page_syntax(std::string error_page_value);

#endif