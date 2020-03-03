#include "RAMMachine.h"

template<class T>
const std::map<InstructionSet, std::string> RAMSimulator<T>::IS = {
	{ InstructionSet::LOAD, "LOAD" },
	{ InstructionSet::STORE, "STORE" },
	{ InstructionSet::ADD, "ADD" },
	{ InstructionSet::SUB, "SUB" },
	{ InstructionSet::MULT, "MULT" },
	{ InstructionSet::DIV, "DIV" },
	{ InstructionSet::READ, "READ" },
	{ InstructionSet::WRITE, "WRITE" },
	{ InstructionSet::JUMP, "JUMP" },
	{ InstructionSet::JGTZ, "JGTZ" },
	{ InstructionSet::JZERO,"JZERO" },
	{ InstructionSet::HALT,"HALT" },
	{ InstructionSet::CONTINUE, "CONTINUE"},
};

template<class T>
const std::map<OperatorDirType, std::string> RAMSimulator<T>::DIRECTION_MODES = {
	{OperatorDirType::UNDEFINED, ""},
	{OperatorDirType::DIRECT, ""},
	{OperatorDirType::INDIRECT, "*" },
	{OperatorDirType::CONSTANT, "="},	
};

template<class T>
void RAMSimulator<T>::loadInputTape(const char* path)
{
	StrReader lector(path);
	std::string content = lector.read();
	lector.close();

	std::istringstream iss(content);
	std::vector<std::string> listContent((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
	iss.clear();
	content.clear();

	for (std::string str : listContent) {
		int value = std::stoi(str);
		this->_pipeIn.push(value);
	}
	listContent.clear();
}

template<class T>
void RAMSimulator<T>::outOutputTape(const char* path)
{
	//ToDo . Escribir contenido en fichero apuntado por path
	Writer<const T> writer(path);
	do{
		writer.write(_pipeOut.pop());
	}while(!_pipeOut.getContent().empty());
	writer.close();
}

template<class T>
RAMSimulator<T>::RAMSimulator(const Tokenizer& compileInfo, const char* inputPathTape, const char* outputPathTape)
{
	this->_memData.reserve(10);
	for(int i = 0; i < 10 ; i++)
	this->_memData.push_back(T());
	this->_memInstruct = compileInfo.getInstructionsInfo();
	this->_pipeIn.setStrategy(new FIFOStrategy<int>());
	this->_pipeOut.setStrategy(new FIFOStrategy<int>());
	this->loadInputTape(inputPathTape);
	this->_outputPathTape = outputPathTape;
}

template<class T>
RAMSimulator<T>::~RAMSimulator()
{
	_pipeIn.clear();
	_memData.clear();
	_memInstruct.clear();
	_pipeOut.clear();
}

template<class T>
void RAMSimulator<T>::showInputTape(std::ostream& out){
	out << "\nInput: { " << std::endl;
	for (const T value : _pipeIn.getContent())
		out << '\t' << value << std::endl;
	out << "}" << std::endl;
}

template<class T>
void RAMSimulator<T>::showInstructions(std::ostream& out){
	out << "\nInstructions: { " << std::endl;
	unsigned int i = 0;
	for (const InstructionToken token : _memInstruct)
		out << '\t' << token.getInstruction() << " " << token.getDirType() << " " << token.getArgument() << (i++ == _pc ? " <-(PC)" : "" )<< std::endl;
	out << "}" << std::endl;
}

template<class T>
void RAMSimulator<T>::showData(std::ostream& out){
	out << "\nData: {" << std::endl;
	for (const T value : this->_memData) out << '\t' << value << std::endl;
	out << "}" << std::endl;
}

template<class T>
void RAMSimulator<T>::showOutputTape(std::ostream& out){
	out << "\nOutput: { " << std::endl;
	for (const T value : _pipeOut.getContent())
		out << '\t' << value << std::endl;
	out << "}" << std::endl;
}

template<class T>
std::ostream& RAMSimulator<T>::operator<<(std::ostream& out)
{
	out << "Estado maquina : " << std::endl;
	showInputTape(out);
	showInstructions(out);
	showData(out);
	showOutputTape(out);

	return out;
}

template<class T>
const std::string RAMSimulator<T>::dessamsembly(const std::vector<InstructionToken>& memInstructions, std::vector<std::string>* lines)
{
	const std::string label = "label";
	unsigned int id = 0;
	std::vector<std::string>* linesTemp = new std::vector<std::string>(memInstructions.size());
	unsigned int i = 0;
	for (const InstructionToken token : memInstructions) {
		char* buffer = new char[255];
		std::string arg(itoa(token.getArgument(), buffer, 10U));
		delete buffer;
		std::string labelId;

		switch (token.getInstruction()) {
			case InstructionSet::CONTINUE:
				//Ignorar
			break;
			case InstructionSet::JUMP:
			case InstructionSet::JGTZ:
			case InstructionSet::JZERO:
				buffer = new char[255];
				labelId = label + std::string(itoa(id++,buffer, 10U)) + ": ";
				delete buffer;
				(*linesTemp)[token.getArgument()] = labelId + (*linesTemp)[token.getArgument()];
				(*linesTemp)[i] += "\t" + std::string(RAMSimulator::IS.find(token.getInstruction())->second + " " +
					RAMSimulator::DIRECTION_MODES.find(token.getDirType())->second +
					labelId + "\n");
			break;
			case InstructionSet::HALT:
				(*linesTemp)[i] += "\t" + std::string(RAMSimulator::IS.find(token.getInstruction())->second + " " +
					RAMSimulator::DIRECTION_MODES.find(token.getDirType())->second + "\n");
				break;
			default: 
				//res += 
				(*linesTemp)[i] += "\t" + std::string(RAMSimulator::IS.find(token.getInstruction())->second + " " +
					RAMSimulator::DIRECTION_MODES.find(token.getDirType())->second +
					arg + "\n");
				break;
		}
		i += 1;
	}
	std::string res;
	for (std::string str : *linesTemp)
		res += str;
	if(lines != nullptr)
		lines->assign(linesTemp->begin(), linesTemp->end());
	linesTemp->clear();
	delete linesTemp;
	return res;
}

template<class T>
void RAMSimulator<T>::execute(unsigned int num){
	bool working = true;
	unsigned int count = 0;
	do{
		T val;
		if(_pc >= _memInstruct.size()) break;
		const InstructionToken& inst = _memInstruct[_pc++];
		switch(inst.getInstruction()){
			case InstructionSet::CONTINUE:
				//Ignorar
			break;
			case InstructionSet::DIV:
				val = getValue(inst.getDirType(),inst.getArgument());
				if(val == 0){
					char* buffer = new char[255];
					sprintf_s(buffer, 255, "Detectado ivision por cero en linea %d",inst.getLine());
					throw Exception(buffer);
				} 
				if(_debug) std::cout << _memData[ACC_INDEX] << " / " << val << " = ";
				this->_memData[ACC_INDEX] = this->_memData[ACC_INDEX] / val;
				if(_debug) std::cout << this->_memData[ACC_INDEX] << std::endl;
			break;
			case InstructionSet::HALT:
				this->outOutputTape(this->_outputPathTape);
				working = false;
			break;
			case InstructionSet::JGTZ:
				if(this->_memData[ACC_INDEX] > 0)
					_pc = inst.getArgument();
			break;
			case InstructionSet::JUMP:
				_pc = inst.getArgument();
			break;
			case InstructionSet::JZERO:
				if(this->_memData[ACC_INDEX] == 0)
					_pc = inst.getArgument();
			break;
			case InstructionSet::LOAD: // Carga en acc valor apuntado
				val = getValue(inst.getDirType(),inst.getArgument());
				if(_debug) std::cout << "mem[0] <= " << val << std::endl;
				this->_memData[ACC_INDEX] = val;
			break;
			case InstructionSet::MULT:
				val = getValue(inst.getDirType(),inst.getArgument());
				if(_debug) std::cout << _memData[ACC_INDEX] << " * " << val << " = ";
				this->_memData[ACC_INDEX] = this->_memData[ACC_INDEX] * val;
				if(_debug) std::cout << this->_memData[ACC_INDEX] << std::endl;
			break;
			case InstructionSet::READ:
				val = this->_pipeIn.pop();
				if(inst.getDirType() == OperatorDirType::DIRECT){
					if(_debug) std::cout << this->_memData[inst.getArgument()]  << " <= " << val << std::endl;
					this->_memData[inst.getArgument()] = val;
				}
				else if(inst.getDirType() == OperatorDirType::INDIRECT){
					if(_debug) std::cout << this->_memData[this->_memData[inst.getArgument()]]  << " <= " << val << std::endl;
					this->_memData[this->_memData[inst.getArgument()]] = val;
				}
					
			break;
			case InstructionSet::STORE:
				if(inst.getDirType() == OperatorDirType::DIRECT){
					if(_debug) std::cout << "mem[" <<inst.getArgument()  << "] <= " << this->_memData[ACC_INDEX] << std::endl;
					this->_memData[inst.getArgument()] = this->_memData[ACC_INDEX];
				}					
				else if(inst.getDirType() == OperatorDirType::INDIRECT){
					if(_debug) std::cout << "mem[*" << inst.getArgument() << "] <= " << this->_memData[ACC_INDEX] << std::endl;
					this->_memData[this->_memData[inst.getArgument()]] = this->_memData[ACC_INDEX];
				}
					
			break;
			case InstructionSet::SUB:
				val = getValue(inst.getDirType(),inst.getArgument());
				if(_debug) std::cout << _memData[ACC_INDEX] << " - " << val << " = ";
				this->_memData[ACC_INDEX] = this->_memData[ACC_INDEX] - val;
				if(_debug) std::cout << this->_memData[ACC_INDEX] << std::endl;
			break;
			case InstructionSet::ADD:
				val = getValue(inst.getDirType(),inst.getArgument());
				if(_debug) std::cout << _memData[ACC_INDEX] << " + " << val << " = ";
				this->_memData[ACC_INDEX] = this->_memData[ACC_INDEX] + val;
				if(_debug) std::cout << this->_memData[ACC_INDEX] << std::endl;
			break;
			case InstructionSet::WRITE:
				val = getValue(inst.getDirType(),inst.getArgument());
				//std::cout << "Insertando : " << val << std::endl; 
				this->_pipeOut.push(val);
			break;
		}

		if(num != 0) working = !(++count == num);

	}while(working);
	if(working){
		std::cerr << "Algo ha salido mal porque se ha sobrepasado la memoria de instrucciones y se ha salido prematuramente..." << std::endl;
	}
}

template<class T>
T RAMSimulator<T>::getValue(OperatorDirType dirType, unsigned int arg){
	T val = arg; //Suponemos de principio que es direccionamiento a constante
	if(dirType == OperatorDirType::DIRECT)
		val = this->_memData[arg];
	else if(dirType	 == OperatorDirType::INDIRECT)
		val = this->_memData[this->_memData[arg]];
	return val;
}

template<class T>
void RAMSimulator<T>::help(std::ostream& out){
	out<< "...:Modo debug:..." << std::endl;
	out << "__________________" << std::endl;
	out << "\t1 - Ver Estado de Memoria" << std::endl;
	out << "\t2 - Traza" << std::endl;
	out << "\t3 - Desamblador" << std::endl;
	out << "\t4 - Ver Cinta de entrada" << std::endl;
	out << "\t5 - Ver Cinta de salida" << std::endl;
	out << "\t6 - Ejecutar" << std::endl;
	out << "\t0 - Salir" << std::endl;
	out << "\th - Ayuda" << std::endl;
	out << "Opcion: ";
}

template<class T>
void RAMSimulator<T>::debug(std::ostream& out){
	
	std::vector<std::string>* lines = new std::vector<std::string>();
	dessamsembly(this->_memInstruct,lines);
	char opt = 0;
	out << std::endl;
	help(out);
	_debug = true;
	do{
		std::cout << "=> ";
		std::cin >> opt;
		switch(opt){
			case '1':
				showData(out);
			break;
			case '2':
				//if(lines == nullptr) out << "Es nulo" << std::endl;
				//else out << "no es nulo y va a buscar en pos " << _pc << ". Ademas, el tamaño es " << lines->size() << std::endl;
				out << "_______________________" << std::endl;
				out << (*lines)[_pc];
				execute(1);
				out << "_______________________" << std::endl;
			break;
			case '3':
				out << dessamsembly(this->_memInstruct);
			break;
			case '4':
				showInputTape(out);
			break;
			case '5':
				showOutputTape(out);
			break;
			case '6':
				_debug = false;
				execute();
			break;
			case 'h':
				help(out);
			break;
			default: //Salir
				_pc = _memInstruct.size();
			break;
		}
	}while(opt != '0' && !finished());
	if(opt != 0) out << "El programa termino. Saliendo de modo debug..." << std::endl;
	lines->clear();
	delete lines;
	_debug = false;
}

template<class T>
bool RAMSimulator<T>::finished(){
	return _pc >= _memInstruct.size();
}

template class RAMSimulator<int>;