#ifndef TOKENIZER_H
#define TOKENIZER_H 1

#include "std.h"
#include "Exception.h"


enum TokenType { COMMENT, LABEL, INSTRUCTION};
enum InstructionSet { LOAD, STORE, ADD, SUB, MULT, DIV, READ, WRITE, JUMP, JGTZ, JZERO, HALT,CONTINUE };
enum OperatorDirType
{
	UNDEFINED = 0,
	DIRECT = 1,
	INDIRECT = 2,
	CONSTANT = 4
};


class Token {
protected:
	TokenType _type ;
	unsigned int _line = 0;
public:
	explicit Token(TokenType type, int line) : _type(type), _line(line){}
	const unsigned int getLine() const { return this->_line; }
};

class CommentToken : public Token {
protected:
	std::string _content;
public:
	explicit CommentToken(const std::string content,unsigned int line) : _content(content),Token(TokenType::COMMENT,line) {};
	std::string getContent() { return this->_content; }
};

class LabelToken : public Token {
protected:
	std::string _label;
public:
	explicit LabelToken(const std::string label, unsigned int line) : _label(label), Token(TokenType::LABEL, line){}
	const std::string getLabel() { return this->_label; }
};

class InstructionToken : public Token {
protected:
	InstructionSet _instruction;
	int _argument; //operando(el valor en si)
	OperatorDirType _dirType; //operador(DIRECTO, INDIRECTO, CONSTANT)
public:
	explicit InstructionToken(InstructionSet instruction, int operand, OperatorDirType operador, int line) : _instruction(instruction), _argument(operand), _dirType(operador), Token(TokenType::INSTRUCTION,line){}
	const InstructionSet getInstruction() const{ return this->_instruction; }
	const int getArgument() const { return this->_argument; }
	const OperatorDirType getDirType() const { return this->_dirType; }
};

//Como no podemos sobrecargar std::istream& operator <<(std::istream&, std::string&). La soluci�n es la siguiente :
class WordDelimitedByNewLine : public std::string {};
//De esta forma, podemos "sobrecargar" el operador y customizarlo a nuestro gusto:
std::istream& operator >> (std::istream& is, WordDelimitedByNewLine& output);
//^Con sobrecargas a este operador, es como vamos a implementar nuestro "split" por '\n'^

class TokenizerBadSyntaxException : public Exception {
public:
	explicit TokenizerBadSyntaxException(const char* msg) : Exception(msg){}
};

class TokenizerDivByZeroException : public Exception {
public:
	explicit TokenizerDivByZeroException() : Exception("Detectado un intento de division por 0") {}
};


class Tokenizer { //Mas bien 'Parseador'
protected:
	std::string _input;
	std::vector<CommentToken> _commentsInfo;
	std::map<std::string, LabelToken> _labelsInfo;
	std::vector<InstructionToken> _instructionsInfo;

	static const std::map<std::string, InstructionSet> IS;
	static const std::map<char, OperatorDirType> DIRECTION_MODES;

public:
	Tokenizer(const std::string input) : _input(input) {}
	std::string sanitaze(const std::string input) const;
	std::string removeComments(const std::string input, std::vector<CommentToken>& comments) const;
	std::string parseLabels(const std::string input, std::vector<LabelToken>& labels) const;
	void parseInstructions(const std::string input, const std::map<std::string, LabelToken>& labelsInfo, std::vector<InstructionToken>& instructions) const;

	Tokenizer& tokenize();

	const std::string getInput() const { return this->_input; }
	const std::vector<CommentToken> getCommentsInfo() const { return this->_commentsInfo; }
	const std::map<std::string, LabelToken> getLabelsInfo() const { return this->_labelsInfo; }
	const std::vector<InstructionToken> getInstructionsInfo() const { return this->_instructionsInfo; }
};


#endif // !TOKENIZER_H
