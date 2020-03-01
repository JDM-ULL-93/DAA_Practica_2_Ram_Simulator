#ifndef RAM_MACHINE_H
#define RAM_MACHINE_H 1

#include "std.h"
#include "Tokenizer.h"
#include "FileHandler.h"

class InstructionToken;
class Tokenizer;

class RAMSimulator { //Prototipo
protected:
	unsigned int _pc = 0;
	std::vector<int> _pipeIn; //Modificar, simplemente leer y cargar en cinta
	std::vector<int> _memData;
	std::vector<InstructionToken> _memInstruct;
	std::vector<int> _pipeOut; //Modificar,simplemente cinta que , en ejecución, se irá populando estilo pila FIFO

	void loadInputTape(const char* path);
	void loadOutputTape(const char* path);

	static const std::map<InstructionSet, std::string> IS;
	static const std::map<OperatorDirType, std::string> DIRECTION_MODES;

public:
	RAMSimulator(const Tokenizer& compileInfo, const char* inputPathTape, const char* outputPathTape);
	~RAMSimulator();

	const std::vector<InstructionToken>& getMemInstructs() const { return this->_memInstruct; }
	
	std::ostream& operator<<(std::ostream & out);

	const std::string dessamsembly(const std::vector<InstructionToken>& memInstruction);

};





#endif // !RAM_MACHINE_H
