// Practica_2_RAM_Simulador.cpp : Defines the entry point for the console application.
//

#include "FileHandler.h"
#include "Tokenizer.h"
#include "RAMMachine.h"


void error(std::ostream& out) {
	out << "Usage:" << std::endl;
	out << "<Path to  Program file> <Path to input tape> <Path to output tape>" << std::endl;
	out << "Version 0.2.3" << std::endl;
}

void help(std::ostream& out) {
	out << "___________________________________" << std::endl;
	out << "Introduce a option : " << std::endl;
	out << "\t1 - Help" << std::endl;
	out << "\t2 - Execute" << std::endl;
	out << "\t3 - Debug" << std::endl;
	out << "\t0 - Exit" << std::endl;
	out << "___________________________________" << std::endl;
	out << "Option : ";
}

int main(int argc, const char** args)
{
	try {
		if (argc < 4) {
			std::cout << "Invalid number of arguments" << std::endl;
			error(std::cout);
			return 0;
		}
		else {
			//StrReader lector("ejemplosRam/test7.ram");
			StrReader lector(args[1]);
			std::string content = lector.read();
			//std::cout << content << std::endl;
			lector.close();
			Tokenizer tokenizer(content);
			RAMSimulator<> ram(tokenizer.tokenize(), args[2], args[3]);
			char opt = 0;
			std::cout << opt << std::endl;
			help(std::cout);
			do {
				std::cout << "=> ";
				std::cin >> opt;
				std::cout << std::endl << std::endl;
				switch (opt) {
					case '1':
					case 'h':
						help(std::cout);
					break;
					case '2':
						ram.execute();
					break;
					case '3':
						ram.debug(std::cout);
					break;
				}
			} while (opt != '0' && !ram.finished());
		}
		/*
		while (!test.empty()) { //Recorrido LIFO
			std::cout << test.back() << std::endl;
			test.pop_back();
		}
		*/
		/*
		while (!test.empty()) { //Recorrido FIFO
			std::cout << test.front() << std::endl;
			test.pop_front();
		}
		*/
	}
	catch (Exception &e) {
		std::cout << e.what() << std::endl;
	}
	system("pause");
    return 0;
}
