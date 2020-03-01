#ifndef RAM_MACHINE_H
#define RAM_MACHINE_H 1

#include "std.h"
#include "Tokenizer.h"
#include "FileHandler.h"


template<class T>
class PushStrategy{
public:
	PushStrategy(){}
	virtual ~PushStrategy(){}
	virtual void push(const T& element, std::list<T>& list) = 0;
	virtual const T& pop(std::list<T>& list) = 0;
};

template<class T>
class FIFOStrategy : public PushStrategy<T>{
public:
	virtual void push(const T& element, std::list<T>& list) override {
		list.push_back(element);
	}

	virtual const T& pop(std::list<T>& list) override {
		const T& element = list.front();
		list.pop_front();
		return element;
	}
};

template <class T>
class Cinta{
protected:
	std::list<T> _content;
	unsigned int _pos = 0;
	PushStrategy<T>* _strategy = nullptr;
public:
	Cinta(){}
	void setStrategy(PushStrategy<T>* strategy){
		if(_strategy != nullptr) delete _strategy;
		this->_strategy = strategy;
	}
	void push(const T element){
		if(_strategy == nullptr) throw Exception("No se ha seteado una estrategia para la clase Cinta");
		_strategy->push(element, _content);
		_pos += 1;
	}
	const T& pop(){
		if(_strategy == nullptr) throw Exception("No se ha seteado una estrategia para la clase Cinta");
		if(!_content.empty()){
			_pos -= 1;
			return _strategy->pop(_content);
		}
		else return 0; //Devolver 0 se computa como final de stream
			//throw Exception("La lista esta vacia, no se puede hacer pop");
	}

	const std::list<T> getContent() const{
		return _content;
	}

	void clear(){
		delete _strategy;
		_content.clear();
		_pos = 0;
	}
	/*
	std::ostream& operator<<(std::ostream & out){
		for(int i = 0,it = _content.begin(); it != _content.end(); it++,i++)
			out << "[" << i << "] : " << *it << std::endl;
	}*/
};


class InstructionToken;
class Tokenizer;

#define ACC_INDEX 0
template <class T = int>
class RAMSimulator {
protected:
	unsigned int _pc = 0;
	Cinta<T> _pipeIn; //Modificar, simplemente leer y cargar en cinta
	std::vector<T> _memData;
	std::vector<InstructionToken> _memInstruct;
	Cinta<T> _pipeOut; //Modificar,simplemente cinta que , en ejecución, se irá populando estilo pila FIFO
	const char* _outputPathTape;
	bool _debug = false;

	void loadInputTape(const char* path);
	void outOutputTape(const char* path);
	T getValue(OperatorDirType dirType, unsigned int arg);

	void showInputTape(std::ostream& out);
	void showInstructions(std::ostream& out);
	void showData(std::ostream& out);
	void showOutputTape(std::ostream& out);
	void help(std::ostream& out);

	static const std::map<InstructionSet, std::string> IS;
	static const std::map<OperatorDirType, std::string> DIRECTION_MODES;

public:
	RAMSimulator(const Tokenizer& compileInfo, const char* inputPathTape, const char* outputPathTape);
	~RAMSimulator();

	const std::vector<InstructionToken>& getMemInstructs() const { return this->_memInstruct; }
	
	std::ostream& operator<<(std::ostream & out);

	const std::string dessamsembly(const std::vector<InstructionToken>& memInstruction,std::vector<std::string>* lines = nullptr);

	void execute(unsigned int num = 0);

	void debug(std::ostream& out);
};





#endif // !RAM_MACHINE_H
