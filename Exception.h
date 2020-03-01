#ifndef EXCEPTION_H
#define EXCEPTION_H 1

#include "std.h"

class Exception : std::exception {
public:
	explicit Exception(const char* message) : _errorMsg(message), std::exception() {}
	virtual ~Exception() {}
	virtual const char* what() const throw ();
protected:
	std::string _errorMsg;
};

class ExceptionErrorNumber : public Exception {
public:
	explicit ExceptionErrorNumber(int errorNumber, const char* msg) : Exception(msg), _errorNumber(errorNumber) {}
	virtual const char* what() const throw ();
private:
	int _errorNumber = 0;
};


#endif // !EXCEPTION_H
