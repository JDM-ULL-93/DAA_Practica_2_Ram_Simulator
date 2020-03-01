#ifndef IFILEHANDLER_H
#define IFILEHANDLER_H 1

#include "std.h"


class IStrategy {
public:
	virtual void* execute(FILE* file) = 0;
};

class IFileHandler {
protected:
	unsigned int _size = 0;
	FILE* _file = nullptr;
	virtual FILE* open(const char* path, const char* openMode) = 0;
	virtual errno_t close() = 0;	
};


#endif // !IFILEHANDLER_H

