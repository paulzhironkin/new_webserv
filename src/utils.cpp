#include "utils.hpp"

static std::string get_value_in_string(int value)
{
	std:: stringstream ss;
	ss << value;
	std::string str_value ;
	ss >> str_value;
	return (str_value);
}

bool find_location(t_single_list & secondList, std::string &elemToFind, void *ptrReponse)
{
	t_single_list::iterator itr_secondList;
	std::map < std::string, std::string > dictionary;
	std::map < std::string, std::string >::iterator itr_dictionary;

	for (itr_secondList = secondList.begin();  itr_secondList != secondList.end(); itr_secondList++)
	{
		dictionary = *itr_secondList; //to get dictionary of each node
		itr_dictionary = dictionary.find("location");// identify port
		if (itr_dictionary != dictionary.end() && elemToFind.compare(itr_dictionary->second) == 0)
		{
			std::map<std::string, std::string> *ptrMap = (std::map<std::string, std::string> *) ptrReponse;
			(*ptrMap) = dictionary;
			return (true);
		}
	}
	return (false);
}

bool isLocation(std::map < std::string, std::string > &dictionary)
{
	std::map < std::string, std::string >::iterator itr_dictionary;
	itr_dictionary = dictionary.find("location");
	if (itr_dictionary != dictionary.end())
	{
		return (true);
	}
	return (false);
}

bool find_directive(t_single_list & secondList, std::string &elemToFind, void *ptrReponse)
{
	t_single_list::iterator itr_secondList;
	std::map < std::string, std::string > dictionary;
	std::map < std::string, std::string >::iterator itr_dictionary;
	std::string *ptrString = NULL;
	itr_secondList = secondList.begin();
	dictionary = *itr_secondList; //to get dictionary of  node
	itr_dictionary = dictionary.find(elemToFind);// identify port
	bool is_location = isLocation(dictionary);
	if (itr_dictionary != dictionary.end() && is_location == false)
	{
		ptrString = (std::string *) ptrReponse;
		*ptrString = itr_dictionary->second;
		return (true);
	}
	return (false);
}

bool getInfo(int port, std::string elemToFind, void *reponse, bool(*callback)(t_single_list & secondList, std::string &elemToFind, void *ptrReponse))
{
	bool has_find = false;
	t_nested_list firstList =  ConfigParser::getList();
	std::string str_port = get_value_in_string(port);//get value of port under string format
	std::map < std::string, std::string > dictionary;
	std::map < std::string, std::string >::iterator itr_dictionary;
	t_single_list::iterator itr_secondList;
	for (t_nested_list::iterator itr_firstList = firstList.begin();  itr_firstList != firstList.end(); itr_firstList++)
	{
		t_single_list & secondList  = *itr_firstList;
		for (itr_secondList = secondList.begin();  itr_secondList != secondList.end(); itr_secondList++)
		{
			dictionary = *itr_secondList; //to get dictionary of each node
			itr_dictionary = dictionary.find("listen");// identify port
			if (itr_dictionary != dictionary.end() && str_port.compare(itr_dictionary->second) == 0)
			{
				has_find = callback(secondList, elemToFind, reponse);
				if (has_find)
					return (has_find);
			}
		}
	}
	return (has_find);	//return false , the element has not be found
}

bool get_location_url(int port, std::string url, void *ptrReponse)
{
	bool		ret;
	char		*token = strtok((char *)url.c_str(), "/");

	while (token != NULL)
	{
		std::string	elemToFind = "/";
		elemToFind.append(token);
		ret = getInfo(port, elemToFind, ptrReponse, find_location);
		if (ret == true)
			return (true);
		token = strtok(NULL, "/");
	}
	return (false);
}

bool is_a_directory(const std::string &s)
{
  	struct stat buffer;
  	return (stat (s.c_str(), &buffer) == 0 && buffer.st_mode & S_IFDIR); // if exist && is a directory return 1
}

bool exists(const std::string &s)
{
	struct stat buffer;
	return (stat (s.c_str(), &buffer) == 0);
}

bool error_page_syntax(std::string error_page_value)
{
	size_t pos = error_page_value.find(" ");
	if (pos != std::string::npos)
	{
		std::string value = error_page_value.substr(pos + 1);
		if (value[0] == '/') /*absolute path start with a slash '/' */
		{
			std::cout << "we received this absolute path == " << value << std::endl;
			std::cout << "error : syntax error absolute path is not accept" << std::endl;
			return (false);
		}
		return (true);
	}

	return (false);
}