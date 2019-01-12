
#include "line_commander.h"
#include "system.h"
#include "utility.h"
#include "path.h"
#include "ntfs.h"
#include <iostream>
#include <string>
#include "exception.h"
#include <cstdlib>

using namespace std;

/** 
* Starts the application.
* @param argc Number of program arguments.
* @param argv Program arguments.
*/
int main(int argc, char *argv[]) {
	if (argc != 2) {
		return 1;
	}

	try {
		System::getSystem().mountNewFileSystem(argv[1]);
	}
	catch (consistency_error &e) {
		cout << "FILE SYSTEM CORRUPTED(souborový systém je poškozen)" << endl;
		exit(EXIT_FAILURE);
	}

	LineCommander commander;
	while (1) {
		commander.inputCommand();
	}

	return 0;
}