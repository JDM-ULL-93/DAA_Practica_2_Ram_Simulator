#include "RAMMachine.h"


const std::map<InstructionSet, std::string> RAMSimulator::IS = {
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
};

const std::map<OperatorDirType, std::string> RAMSimulator::DIRECTION_MODES = {
	{OperatorDirType::INDIRECT, "*" },
	{OperatorDirType::CONSTANT, "="},
	{OperatorDirType::DIRECT, ""},
	{OperatorDirType::UNDEFINED, ""}
};

void RAMSimulator::loadInputTape(const char* path)
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
		this->_pipeIn.push_back(value);
	}
	listContent.clear();
}

void RAMSimulator::loadOutputTape(const char* path)
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
		this->_pipeOut.push_back(value);
	}
	listContent.clear();
}

RAMSimulator::RAMSimulator(const Tokenizer& compileInfo, const char* inputPathTape, const char* outputPathTape)
{
	this->_memInstruct = compileInfo.getInstructionsInfo();
	this->loadInputTape(inputPathTape);
	this->loadOutputTape(outputPathTape);

}

RAMSimulator::~RAMSimulator()
{
	_pipeIn.clear();
	_memData.clear();
	_memInstruct.clear();
	_pipeOut.clear();
}

std::ostream& RAMSimulator::operator<<(std::ostream& out)
{
	out << "Estado maquina : " << std::endl;
	out << "Input: { " << std::endl;
	for (int value : _pipeIn)
		out << '\t' << value << std::endl;
	out << "}" << std::endl;

	out << "Instructions: { " << std::endl;
	unsigned int i = 0;
	for (InstructionToken token : _memInstruct)
		out << '\t' << token.getInstruction() << " " << token.getDirType() << " " << token.getArgument() << (i++ == _pc ? " <-(PC)" : "" )<< std::endl;
	out << "}" << std::endl;

	out << "Data: { " << std::endl;
	for (int value : _memData)
		out << '\t' << value << std::endl;
	out << "}" << std::endl;

	out << "Output: { " << std::endl;
	for (int value : _pipeOut)
		out << '\t' << value << std::endl;
	out << "}" << std::endl;

	return out;
}

const std::string RAMSimulator::dessamsembly(const std::vector<InstructionToken>& memInstructions)
{
	const std::string label = "label";
	unsigned int id = 0;
	std::vector<std::string> lines(memInstructions.size());
	unsigned int i = 0;
	for (const InstructionToken token : memInstructions) {
		char* buffer = new char[255];
		std::string arg(itoa(token.getArgument(), buffer, 10U));
		delete buffer;
		std::string labelId;

		switch (token.getInstruction()) {
			case InstructionSet::JUMP:
			case InstructionSet::JGTZ:
			case InstructionSet::JZERO:
				buffer = new char[255];
				labelId = label + std::string(itoa(id++,buffer, 10U)) + ": ";
				delete buffer;
				lines[token.getArgument()] = labelId + lines[token.getArgument()];
				lines[i] += "\t" + std::string(RAMSimulator::IS.find(token.getInstruction())->second + " " +
					RAMSimulator::DIRECTION_MODES.find(token.getDirType())->second +
					labelId + "\n");
			break;
			case InstructionSet::HALT:
				lines[i] += "\t" + std::string(RAMSimulator::IS.find(token.getInstruction())->second + " " +
					RAMSimulator::DIRECTION_MODES.find(token.getDirType())->second + "\n");
				break;
			default: 
				//res += 
				lines[i] += "\t" + std::string(RAMSimulator::IS.find(token.getInstruction())->second + " " +
					RAMSimulator::DIRECTION_MODES.find(token.getDirType())->second +
					arg + "\n");
				break;
		}
		i += 1;
	}
	std::string res;
	for (std::string str : lines)
		res += str;
	lines.clear();
	return res;
}

