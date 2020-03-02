#include "FileHandler.h"

FileHandler::~FileHandler()
{
	_size = 0;
	if (_file != nullptr)
		this->close();
}

FileHandler::FileHandler(const char* path,const char* openMode)
{
	_file = this->open(path, openMode);
	std::size_t found = std::string(openMode).find('r');
	if(found!=std::string::npos)_size = this->calcSize();
}

FILE* FileHandler::open(const char* path,const char* openMode) {
	FILE* file = new FILE();
	int errNo = fopen_s(&file,path, openMode);
	if (file == nullptr)
		throw FileHandlerOpenException(errNo);
	return file;
}

const long long FileHandler::calcSize()
{
	fpos_t begin = 0;
	fseek(_file, 0, SEEK_END);
	fpos_t end; fgetpos(_file, &end);
	fsetpos(_file, &begin);
	return end;
}

errno_t FileHandler::close() {
	int errorNumber = fclose(_file);
	_file = nullptr;
	return errorNumber;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////


const char* Reader<const char*>::read()
{ 
	fpos_t begin = 0;
	fsetpos(_file, &begin);
	//TODO: 
	//Tratar el problema de ficheros que excedan el tama�o de un entero (con lo cual habr�a que repetir este proceso de lectura varias veces)
	try {
		char* buffer = new char[_size+1];
		for (unsigned int i = 0; i < _size; i++)
			buffer[i] = fgetc(_file);
		buffer[_size] = '\0';
		return buffer;
	}
	catch (...) {
		if (feof(_file))
			throw ReaderEOFException();
		else
			throw ReaderErrorException(ferror(_file));
	}
}


const wchar_t* Reader<const wchar_t*>::read()
{
	fpos_t begin = 0;
	fsetpos(_file, &begin);
	//TODO: 
	//Tratar el problema de ficheros que excedan el tama�o de un entero (con lo cual habr�a que repetir este proceso de lectura varias veces)
	try {
		wchar_t* buffer = new wchar_t[_size+1];
		for (unsigned int i = 0; i < _size; i++)
			buffer[i] = fgetwc(_file);
		buffer[_size] = '\0';
		return buffer;
	}
	catch (...) {
		if (feof(_file))
			throw ReaderEOFException();
		else
			throw ReaderErrorException(ferror(_file));
	}
}

const std::string& Reader<const std::string&>::read()
{
	fpos_t begin = 0;
	fsetpos(_file, &begin);
	//TODO: 
	//Tratar el problema de ficheros que excedan el tama�o de un entero (con lo cual habr�a que repetir este proceso de lectura varias veces)
	try {
		char* buffer = new char[_size+1];
		for (unsigned int i = 0; i < _size; i++)
			buffer[i] = fgetc(_file);
		buffer[_size] = '\0';
		std::string* result = new std::string(buffer);
		delete buffer;
		return *result;
	}
	catch (...) {
		if (feof(_file))
			throw ReaderEOFException();
		else
			throw ReaderErrorException(ferror(_file));
	}

}


void Writer<const std::string&>::write(const std::string& data){
	fwrite(data.data(),sizeof(char),data.size(),_file);
	int error = ferror(_file);
	if(error) throw WriterErrorException(error);

}

void Writer<const int>::write(const int data){
	char* buffer = new char[_MAX_PATH];
	sprintf(buffer,"%d, ",data);
	fputs(buffer,_file);
	int error = ferror(_file);
	if(error) throw WriterErrorException(error);

}


//template class Reader<const char*>;
//template class Reader<const unsigned char*>;