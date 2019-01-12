
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "utility.h"
#include "line_commander.h"
#include "path.h"
#include "file.h"
#include "file_system.h"
#include "system.h"
#include "directory.h"
#include "exception.h"

#if defined(_WIN32)
#include <windows.h>
#endif

const map<string, LineCommander::command> LineCommander::map_command = LineCommander::createMap();

/*
* __________________________________PUBLIC FUNCTIONS______________________________________
*/

/**
* Reads a new command from a user.
*/
void LineCommander::inputCommand() {
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail(), '\n');
	string command_input;

#if defined(_WIN32)
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, 10);
	cout << this->getActualDir() + " >> ";
	SetConsoleTextAttribute(console, 15);
#elif defined(__unix__) || defined(__CYGWIN__)
	cout << "\033[32m" << this->getActualDir() << " >> " << "\033[0m";
#endif
	
	getline(cin, command_input);
	this->parseCommand(trim(command_input));
}

/*
* __________________________________PROTECTED FUNCTIONS______________________________________
*/

/**
* Parses an input from a user and calls a corresponding command.
* @param command_input The input command
*/
void LineCommander::parseCommand(string command_input) {
	if (command_input.empty()) {
		return;
	}

	vector<string> command_parts;
	split(command_input, " ", command_parts);

	map<string, command>::const_iterator it = this->map_command.find(command_parts.front());

	if (it != map_command.end()) {
		command func = it->second;
		(this->*func)(command_parts);
	}
	else {
		cout << "Command not found. For help type \"help\"" << endl;
	}
}

/*
* __________________________________PRIVATE FUNCTIONS______________________________________
*/

/**
* Copies s1 to s2.
* @param command Parts of a command.
*/
void LineCommander::cp(const vector<string> &command_parts) {
	if (command_parts.size() != 3) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}	

	try {
		File file(Path(command_parts[1], this->getActualDir()));
		file.open("r");
		file.copy(Path(command_parts[2], this->getActualDir()));
		cout << "OK" << endl;
	}
	catch (file_not_found &e) {
		cout << "FILE NOT FOUND (není zdroj)" << endl;
	}
	catch (path_not_found &e) {
		cout << "PATH NOT FOUND (neexistuje cílová cesta)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
	
}

/**
* Moves file s1 to s2.
* @param command Parts of a command.
*/
void LineCommander::mv(const vector<string> &command_parts) {
	if (command_parts.size() != 3) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}
	try {
		File file(Path(command_parts[1], this->getActualDir()));
		file.open("r");
		file.move(Path(command_parts[2], this->getActualDir()));
		cout << "OK" << endl;
	}
	catch (file_not_found &e) {
		cout << "FILE NOT FOUND (není zdroj)" << endl;
	}
	catch (path_not_found &e) {
		cout << "PATH NOT FOUND (neexistuje cílová cesta)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
}

/**
* Deletes file s1.
* @param command Parts of a command.
*/
void LineCommander::rm(const vector<string> &command_parts) {
	if (command_parts.size() != 2) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	try {
		File file(Path(command_parts[1], this->getActualDir()));
		file.open("r");
		file.remove();
		cout << "OK" << endl;
	}
	catch (file_not_found &e) {
		cout << "FILE NOT FOUND" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
	
}

/**
* Creates directory a1.
* @param command Parts of a command.
*/
void LineCommander::mkdir(const vector<string> &command_parts) {
	if (command_parts.size() != 2) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}
	try {
		Directory dir(Path(command_parts[1], this->getActualDir()));
		dir.open("w");
		cout << "OK" << endl;
	}
	catch (file_not_found &e) {
		cout << "PATH NOT FOUND (neexistuje zadaná cesta)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
	catch (alloc_error &e) {
		cout << "EXIST (nelze založit, již existuje)" << endl;
	}
}

/**
* Deletes empty directory a1.
* @param command Parts of a command.
*/
void LineCommander::rmdir(const vector<string> &command_parts) {
	if (command_parts.size() != 2) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	try {
		Directory dir(Path(command_parts[1], this->getActualDir()));
		dir.open("r");
		dir.remove();
		cout << "OK" << endl;
	}
	catch (file_not_found &e) {
		cout << "PATH NOT FOUND (neexistuje zadaná cesta)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
	catch (delete_error &e) {
		cout << "NOT EMPTY (adresáø obsahuje podadresáøe, nebo soubory)" << endl;
	}
	
}

/**
* Shows content of the a1 directory.
* @param command Parts of a command.
*/
void LineCommander::ls(const vector<string> &command_parts) {
	try {
		if (command_parts.size() > 2) {
			cout << "Wrong number of arguments. For help type \"help\"" << endl;
			return;
		}
		else if (command_parts.size() == 1) {
			Directory dir(Path(this->getActualDir()));
			dir.open("r");
			cout << dir.read();
		}
		else {
			Directory dir(Path(command_parts[1], this->getActualDir()));
			dir.open("r");
			cout << dir.read();
		}
	}
	catch (file_not_found &e) {
		cout << "PATH NOT FOUND (neexistující adresáø)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
	
}

/**
* Shows content of the s1 file.
* @param command Parts of a command.
*/
void LineCommander::cat(const vector<string> &command_parts) {
	if (command_parts.size() != 2) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	try {
		File file(Path(command_parts[1], this->getActualDir()));
		file.open("r");
		cout << file.read() << endl;
	}
	catch (file_not_found &e) {
		cout << "FILE NOT FOUND (není zdroj)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
	
}

/**
* Changes the current path to a1 directory.
* @param command Parts of a command.
*/
void LineCommander::cd(const vector<string> &command_parts) {
	try {
		if (command_parts.size() > 2) {
			cout << "Wrong number of arguments. For help type \"help\"" << endl;
			return;
		}
		else if (command_parts.size() == 1) {
			Directory dir(Path(this->getActualDir()));
			dir.open("r");
			this->setActualDir('/' + dir.getPath().getPathParts().front());
		}
		else {
			Directory dir(Path(command_parts[1], this->getActualDir()));
			dir.open("r");
			this->setActualDir(dir.getPath().toString());
		}

		cout << "OK" << endl;
	}
	catch (file_not_found &e) {
		cout << "PATH NOT FOUND (neexistující cesta)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
	
}
/**
* Shows the current path
* @param command Parts of a command.
*/
void LineCommander::pwd(const vector<string> &command_parts) {
	if (command_parts.size() != 1) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	cout << this->getActualDir() << endl;
}

/**
* Shows s1/a1 file/directory information (in which fragments / clusters it is located), uid,...
* @param command Parts of a command.
*/
void LineCommander::info(const vector<string> &command_parts) {
	if (command_parts.size() != 2) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}
	
	try {
		try {
			File file(Path(command_parts[1], this->getActualDir()));
			file.open("r");
			cout << file.info();
		}
		catch (file_not_found &e) {
			Directory dir(Path(command_parts[1], this->getActualDir()));
			dir.open("r");
			cout << dir.info();
		}
		catch (out_of_range &e) {
			cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
		}
	}
	catch (file_not_found &e) {
		cout << "FILE NOT FOUND (není zdroj)" << endl;
	}
	
	
}

/**
* Uploads file s1 from hard drive to s2 in pseudoNTFS.
* @param command Parts of a command.
*/
void LineCommander::incp(const vector<string> &command_parts) {
	if (command_parts.size() != 3) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	try {
		File file(Path(command_parts[2], this->getActualDir()));
		file.open("w");

		string line;
		ifstream uploaded_file(command_parts[1].data());
		if (uploaded_file.is_open())
		{
			while (getline(uploaded_file, line))
			{
				file.write(line + '\n');
			}

			cout << "OK" << endl;
			uploaded_file.close();
		}
		else {
			cout << "FILE NOT FOUND(není zdroj)" << endl;
		}
	}
	catch (file_not_found &e) {
		cout << "PATH NOT FOUND (neexistuje cílová cesta)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
}

/**
* Uploads s1 from pseudoNTFS to s2 on hard disk.
* @param command Parts of a command.
*/
void LineCommander::outcp(const vector<string> &command_parts) {
	if (command_parts.size() != 3) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}
	try {
		File file(Path(command_parts[1], this->getActualDir()));
		file.open("r");

		ofstream download_file(command_parts[2].data());
		if (download_file.is_open())
		{
			download_file << file.read() << '\n';

			cout << "OK" << endl;
			download_file.close();
		}
		else {
			cout << "PATH NOT FOUND (neexistuje cílová cesta)" << endl;
		}
	}
	catch (file_not_found &e) {
		cout << "FILE NOT FOUND (není zdroj)" << endl;
	}
	catch (out_of_range &e) {
		cout << "NAME NOT VALID (pøíliš dlouhý název, max 12)" << endl;
	}
}

/**
* Defragments a file system.
* @param command Parts of a command.
*/
void LineCommander::defrag(const vector<string> &command_parts) {
	if (command_parts.size() != 2) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	FileSystem* file_system = System::getSystem().getFileSystem(command_parts[1]);
	if (file_system != NULL) {
		file_system->defragment();
		cout << "OK" << endl;
	}
	else {
		cout << "FILE SYSTEM NOT FOUND(souborový systém neexistuje)" << endl;
	}
}

/**
* Checks status of a file system.
* @param command Parts of a command.
*/
void LineCommander::checkdisk(const vector<string> &command_parts) {
	if (command_parts.size() != 2) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	try {
		FileSystem* file_system = System::getSystem().getFileSystem(command_parts[1]);
		if (file_system != NULL) {
			file_system->checkConsistency();
			cout << "OK" << endl;
		}
		else {
			cout << "FILE SYSTEM NOT FOUND(souborový systém neexistuje)" << endl;
		}
	}
	catch (consistency_error &e) {
		cout << "FILE SYSTEM CORRUPTED(souborový systém je poškozen)" << endl;
		exit(EXIT_FAILURE);
	}
	

	
}

/**
* Loads the file from the hard drive where the commands are prepared and starts doing them sequentially. The format is one command per line.
* @param command Parts of a command.
*/
void LineCommander::load(const vector<string> &command_parts) {
	if (command_parts.size() != 2) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	string line;
	ifstream commands_file(command_parts[1].data());
	if (commands_file.is_open())
	{
		while (getline(commands_file, line))
		{
			this->parseCommand(trim(line));
		}

		cout << "OK" << endl;
		commands_file.close();
	}
	else {
		cout << "FILE NOT FOUND(není zdroj)" << endl;
	}
}

/**
* Shows help.
* @param command Parts of a command.
*/
void LineCommander::help(const vector<string> &command_parts) {
	if (command_parts.size() != 1) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	cout << "The program is controlled by the following commands : \n"
		<< "cp s1 s2 \t Copies s1 to s2 \n"
		<< "mv s1 s2 \t Moves file s1 to s2 \n"
		<< "rm s1 \t \t Deletes file s1 \n"
		<< "mkdir a1 \t Creates directory a1 \n"
		<< "rmdir a1 \t Deletes empty directory a1 \n"
		<< "ls a1 \t \t Shows content of the a1 directory \n"
		<< "cat s1 \t \t Shows content of the s1 file \n"
		<< "cd a1 \t \t Changes the current path to a1 directory \n"
		<< "pwd \t \t Shows the current path \n"
		<< "info a1/s1 \t Shows s1/a1 file/directory information (in which fragments / clusters it is located), uid,... \n"
		<< "incp s1 s2 \t Uploads file s1 from hard drive to s2 in pseudoNTFS \n"
		<< "outcp s1 s2 \t Uploads s1 from pseudoNTFS to s2 on hard disk \n"
		<< "defrag s1 \t Deframents selected file system \n"
		<< "checkdisk s1 \t Checks selected file system for errors \n"
		<< "load s1 \t Loads the file from the hard drive where the commands are prepared and starts doing them sequentially. The format is one command per line \n"
		<< "help \t \t Shows this help \n" 
		<< "quit \t \t Closes the application" << endl;
}

/**
* Exits the application.
* @param command Parts of a command.
*/
void LineCommander::quit(const vector<string> &command_parts) {
	if (command_parts.size() != 1) {
		cout << "Wrong number of arguments. For help type \"help\"" << endl;
		return;
	}

	exit(EXIT_SUCCESS);
}
