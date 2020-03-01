#include "Tokenizer.h"

const std::map<std::string, InstructionSet> Tokenizer::IS = {
	{ "LOAD", InstructionSet::LOAD },
	{ "STORE", InstructionSet::STORE },
	{ "ADD", InstructionSet::ADD },
	{ "SUB", InstructionSet::SUB },
	{ "MULT", InstructionSet::MULT },
	{ "DIV", InstructionSet::DIV },
	{ "READ", InstructionSet::READ },
	{ "WRITE", InstructionSet::WRITE },
	{ "JUMP", InstructionSet::JUMP },
	{ "JGTZ", InstructionSet::JGTZ },
	{ "JZERO", InstructionSet::JZERO },
	{ "HALT", InstructionSet::HALT },
};

const std::map<char, OperatorDirType> Tokenizer::DIRECTION_MODES = {
	{'*', OperatorDirType::INDIRECT},
	{'=', OperatorDirType::CONSTANT}
};

std::istream& operator >> (std::istream& is, WordDelimitedByNewLine& output)
{
	std::getline(is, output, '\n');
	if (output.empty()) {
		std::getline(is, output, '\n');
		/*output.clear();
		std::string str("HALT");
		output.replace(output.begin(), output.end(), str.begin(), str.end());*/
	}
	return is;

	///USAGE:
	/*
	std::istringstream iss(copyInput);
	std::vector<std::string> results((std::istream_iterator<WordDelimitedByPad>(iss)),
	std::istream_iterator<WordDelimitedByPad>());
	for (std::string str : results)
	std::cout << str << std::endl;
	*/
}

std::string Tokenizer::sanitaze(const std::string input) const
{
	std::string result = input;
	result.erase(std::remove(result.begin(), result.end(), '\t'), result.end());
	result.erase(std::remove(result.begin(), result.end(), '\r'), result.end()); //WINDOWS solo
	return result;
}

std::string Tokenizer::removeComments(const std::string input, std::vector<CommentToken>& comments) const
{
	std::string result;

	std::istringstream iss(input);
	std::vector<std::string> lines((std::istream_iterator<WordDelimitedByNewLine>(iss)), std::istream_iterator<WordDelimitedByNewLine>());
	for (unsigned int i = 0; i < lines.size(); i++) {
		if (lines[i][0] == ';' || lines[i][0] == '#') //Empieza con un caracter de comentario,  es un comentario
			comments.push_back(CommentToken(lines[i].substr(1U,lines[i].size()-1), i+1));
		else {//No empieza con caracter de comentario, pero lo puede tener mas adelante
			char* nextToken;
			char* token = strtok_s(const_cast<char*>(lines[i].c_str()), "#;", &nextToken);
			result += std::string(token) + ";\n"; // Marcamos con ';' el final de linea, esto será útil más adelante; (ver linea 75)
			if(!std::string(nextToken).empty()) comments.push_back(CommentToken(nextToken, i+1));
		}
	}
	//Convertirmos todo lo que sobra a mayúsculas:
	std::use_facet<std::ctype<char>>(std::locale()).toupper(&result[0], &result[0] + result.size());
	return result;
}

std::string Tokenizer::parseLabels(const std::string input, std::vector<LabelToken>& labels) const
{
	std::string result;
	std::istringstream iss(input);
	std::vector<std::string> lines((std::istream_iterator<WordDelimitedByNewLine>(iss)), std::istream_iterator<WordDelimitedByNewLine>());
	for (unsigned int i = 0; i < lines.size(); i++) {
		char* nextToken;
		char* token = strtok_s(const_cast<char*>(lines[i].c_str()), ":", &nextToken);
		//std::cout << token << "||" << nextToken << std::endl;
		if (*nextToken == ';' || *nextToken != 0 ) { //Significa que hay una instrucción. Para esto añadimos el ';' al final
			std::string instruction(nextToken);
			instruction.pop_back(); //Sacamos el ';'
			if (!instruction.empty()) {//instruction = "HALT"; //El caso de que pongamos un label y no haya una sentencia despues, la rellenamos con sentencia 'nula' (= HALT)
				result += instruction + "\n";
				labels.push_back(LabelToken(token, i));
				instruction.clear();
			}
		}
		else {
			std::string instruction(token);
			instruction.pop_back(); //Sacamos el ';'
			result += instruction + "\n";
			instruction.clear();
		}
			
	}
	return result;
}

void Tokenizer::parseInstructions(const std::string input, const std::map<std::string, LabelToken>& labelsInfo, std::vector<InstructionToken>& instructions) const
{
	std::string result;
	std::istringstream iss(input);
	std::vector<std::string> lines((std::istream_iterator<WordDelimitedByNewLine>(iss)), std::istream_iterator<WordDelimitedByNewLine>());
	for (unsigned int i = 0; i < lines.size(); i++){
		std::istringstream iss(lines[i]);
		std::vector<std::string> op((std::istream_iterator<std::string>(iss)), std::istream_iterator<std::string>());
		//op[0] -> la instruccion
		//op[1] -> el argumento
		auto itInstruction = Tokenizer::IS.find(op[0]);
		if (itInstruction == Tokenizer::IS.end()) {
			char* buffer = new char[255];
			sprintf_s(buffer, 255, "Error en sintaxis. Se esperaba una instruccion pero se leyo %s en linea %d", lines[i].c_str(), i + 1);
			throw TokenizerBadSyntaxException(buffer);
		}
		else { //ToFix: Falla porque en una instrucción JUMP, lo que tenemos no es un entero, si no un label
				//IDEA: Para solucionarlo, consultar los labels, obtener el label que corresponde y sustituirlo por la linea en la que se encuentra
			//std::cout << op[1][0] << std::endl;
			auto itLabel = labelsInfo.end();
			auto itDir = Tokenizer::DIRECTION_MODES.end();
			OperatorDirType dirType;
			switch (itInstruction->second) {
				case InstructionSet::JUMP:
				case InstructionSet::JGTZ:
				case InstructionSet::JZERO:
					itLabel = labelsInfo.find(op[1]);
					if (itLabel == labelsInfo.end()) {
						char* buffer = new char[255];
						sprintf_s(buffer, 255, "Error en sintaxis. Se esperaba un label valido pero se encontro un label (%s) no definido en linea %d", op[1].c_str(), i + 1);
						throw TokenizerBadSyntaxException(buffer);
					}
					instructions.push_back(InstructionToken(itInstruction->second, itLabel->second.getLine()-1, OperatorDirType::UNDEFINED, i + 1));			
				break;
				case InstructionSet::HALT:
					instructions.push_back(InstructionToken(InstructionSet::HALT, 0, OperatorDirType::UNDEFINED, i + 1));
				break;
				case InstructionSet::DIV:
					itDir = Tokenizer::DIRECTION_MODES.find(op[1][0]); //Si empieza por * es direccionamiento indirecto, por = es a constante, e.o.c es direccionamiento directo.
					dirType = itDir == DIRECTION_MODES.end() ? OperatorDirType::DIRECT : itDir->second;
					if (dirType != OperatorDirType::DIRECT) op[1].erase(0, 1);//Eliminamos el operador de direccionamiento
					if (std::stoi(op[1]) == 0) throw TokenizerDivByZeroException();
					instructions.push_back(InstructionToken(itInstruction->second, std::stoi(op[1]), dirType, i + 1));
				break;
				default:
					itDir = Tokenizer::DIRECTION_MODES.find(op[1][0]); //Si empieza por * es direccionamiento indirecto, por = es a constante, e.o.c es direccionamiento directo.
					dirType = itDir == DIRECTION_MODES.end() ? OperatorDirType::DIRECT : itDir->second;
					if(dirType != OperatorDirType::DIRECT) op[1].erase(0, 1);//Eliminamos el operador de direccionamiento
					instructions.push_back(InstructionToken(itInstruction->second, std::stoi(op[1]), dirType, i + 1));
				break;
			}
			
		}
	}
}

Tokenizer& Tokenizer::tokenize()
{
	std::string res = this->sanitaze(this->_input);
	//std::cout << res << std::endl;

	res = this->removeComments(res, _commentsInfo);
	//std::cout << res << std::endl;

	std::vector<LabelToken> listLabels;
	res = this->parseLabels(res, listLabels);
	//std::cout << res << std::endl;

	for (LabelToken token : listLabels)
		this->_labelsInfo.insert(std::make_pair(token.getLabel(), token));

	this->parseInstructions(res, _labelsInfo, _instructionsInfo);
	/*
	for (InstructionToken token : _instructionsInfo)
		std::cout << token.getInstruction() << " " << token.getDirType() << " " << token.getArgument() << std::endl;
	*/
	return *this;
}


