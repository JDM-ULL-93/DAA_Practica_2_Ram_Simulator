#include "Exception.h"

const char * Exception::what() const throw()
{
	return _errorMsg.c_str();
}

const char * ExceptionErrorNumber::what() const throw()
{
	const size_t SIZE = 30;
	char* buffer = new char[SIZE]; memset(buffer, '\0', SIZE);
	size_t written = sprintf_s(buffer, SIZE, ". Error Number: %d\0", _errorNumber);
	std::string* result = new std::string(_errorMsg);
	*result += std::string(buffer);
	delete buffer;
	return (*result).data();
}