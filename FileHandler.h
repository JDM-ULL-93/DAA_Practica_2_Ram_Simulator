#ifndef FILEHANDLER_H
#define FILEHANDLER_H 1

#include "std.h"

#include "Exception.h"
#include "IFileHandler.h"

class FileHandlerOpenException : public ExceptionErrorNumber {
public:
	explicit FileHandlerOpenException(int errorNumber) : ExceptionErrorNumber(errorNumber,"Error intentando abrir el fichero"){}
};

class FileHandler : public IFileHandler {
private:
	const long long calcSize();
	FILE* open(const char* path, const char* openMode) override;
protected:
	explicit FileHandler(const char* path, const char* openMode);
public:
	virtual ~FileHandler(); 
	errno_t close() override;
	

};

class ReaderEOFException : public Exception {
public:
	explicit ReaderEOFException() : Exception("EOF encontrado. No se ha podido leer nada") {}
};

class ReaderErrorException : public ExceptionErrorNumber {
public:
	explicit ReaderErrorException(int errorNumber) : ExceptionErrorNumber(errorNumber, "EOF encontrado. No se ha podido leer nada") {}
};

// SOLID
// SRP -> Responsabilidad Unica: Traer al programa el contenido de un fichero
// OCP ->
template <class T>
class Reader : public FileHandler{
protected:
	explicit Reader<T>(const char* path) : FileHandler(path, "rb") {}
public:
	T read();
};


class SReader : public Reader<const char*>
{
public:
	explicit SReader(const char* path) : Reader(path) {}
};

class WReader : public Reader<const wchar_t*>
{
public:
	explicit WReader(const char* path) : Reader(path) {}
};

class StrReader : public Reader<const std::string&> {
public:
	explicit StrReader(const char* path) : Reader(path) {}
};

#endif // !FILEHANDLER_H

