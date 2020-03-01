// Practica_2_RAM_Simulador.cpp : Defines the entry point for the console application.
//

#include "FileHandler.h"
#include "Tokenizer.h"
#include "RAMMachine.h"


void error() {
	std::cout << "Usage:" << std::endl;
	std::cout << "<Path to  Program file> <Path to input tape> <Path to output tape>" << std::endl;
	std::cout << "Version 0.2.3" << std::endl;
}

void help() {
	std::cout << "ToDo: Implementar ayuda" << std::endl;
}

int main(int argc, const char** args)
{
	try {
		if (argc < 4) {
			std::cout << "Invalid number of arguments" << std::endl;
			error();
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
			int opt = 0;
			do {
				opt = 4;
				std::cout << "___________________________________" << std::endl;
				std::cout << "Introduce a option : " << std::endl;
				std::cout << "\t1 - Help" << std::endl;
				std::cout << "\t2 - Dissasembly" << std::endl;
				std::cout << "\t3 - Show machine state" << std::endl;
				std::cout << "\t4 - Execute" << std::endl;
				std::cout << "\t5 - Debug" << std::endl;
				std::cout << "\t0 - Exit" << std::endl;
				std::cout << "___________________________________" << std::endl;
				std::cout << "Option : ";
				std::cin >> opt;
				std::cout << std::endl << std::endl;
				switch (opt) {
					case 1:
						help();
					break;
					case 2:
						std::cout << ram.dessamsembly(ram.getMemInstructs()) << std::endl;
					break;
					case 3:
						ram << std::cout;
					break;
					case 4:
						ram.execute();
					break;
					case 5:
						ram.debug(std::cout);
					break;
				}
				
			} while (opt != 0);
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

