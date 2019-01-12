#ifndef LINE_COMMANDER_H
#define LINE_COMMANDER_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include "commander.h"

using namespace std;

class LineCommander : public Commander {
public:
	/**
	* Reads a new command from a user.
	*/
	void inputCommand();
protected:
	/**
	* Parses a command from a user
	*/
	void parseCommand(string command_input);

private:

	/** Type of a pointer to command function. */
	typedef void (LineCommander::*command)(const vector<string> &);

	static map<string, command> createMap() {
		map<string, command> temp_map;
		temp_map["cp"] = &LineCommander::cp;
		temp_map["mv"] = &LineCommander::mv;
		temp_map["rm"] = &LineCommander::rm;
		temp_map["mkdir"] = &LineCommander::mkdir;
		temp_map["rmdir"] = &LineCommander::rmdir;
		temp_map["ls"] = &LineCommander::ls;
		temp_map["cat"] = &LineCommander::cat;
		temp_map["cd"] = &LineCommander::cd;
		temp_map["pwd"] = &LineCommander::pwd;
		temp_map["info"] = &LineCommander::info;
		temp_map["incp"] = &LineCommander::incp;
		temp_map["outcp"] = &LineCommander::outcp;
		temp_map["load"] = &LineCommander::load;
		temp_map["defrag"] = &LineCommander::defrag;
		temp_map["checkdisk"] = &LineCommander::checkdisk;
		temp_map["help"] = &LineCommander::help;
		temp_map["quit"] = &LineCommander::quit;
		return temp_map;
	}

	static const map<string, command> map_command;

	/**
	* Copies s1 to s2.
	* @param command Parts of a command.
	*/
	void cp(const vector<string> &command_parts);

	/**
	* Moves file s1 to s2.
	* @param command Parts of a command.
	*/
	void mv(const vector<string> &command_parts);

	/**
	* Deletes file s1.
	* @param command Parts of a command.
	*/
	void rm(const vector<string> &command_parts);

	/**
	* Creates directory a1.
	* @param command Parts of a command.
	*/
	void mkdir(const vector<string> &command_parts);

	/**
	* Deletes empty directory a1.
	* @param command Parts of a command.
	*/
	void rmdir(const vector<string> &command_parts);

	/**
	* Shows content of the a1 directory.
	* @param command Parts of a command.
	*/
	void ls(const vector<string> &command_parts);

	/**
	* Shows content of the s1 file.
	* @param command Parts of a command.
	*/
	void cat(const vector<string> &command_parts);

	/**
	* Changes the current path to a1 directory.
	* @param command Parts of a command.
	*/
	void cd(const vector<string> &command_parts);

	/**
	* Shows the current path
	* @param command Parts of a command.
	*/
	void pwd(const vector<string> &command_parts);

	/**
	* Shows s1/a1 file/directory information (in which fragments / clusters it is located), uid,...
	* @param command Parts of a command.
	*/
	void info(const vector<string> &command_parts);

	/**
	* Uploads file s1 from hard drive to s2 in pseudoNTFS.
	* @param command Parts of a command.
	*/
	void incp(const vector<string> &command_parts);

	/**
	* Uploads s1 from pseudoNTFS to s2 on hard disk.
	* @param command Parts of a command.
	*/
	void outcp(const vector<string> &command_parts);

	/**
	* Defragments a file system.
	* @param command Parts of a command.
	*/
	void defrag(const vector<string> &command_parts);

	/**
	* Checks status of a file system.
	* @param command Parts of a command.
	*/
	void checkdisk(const vector<string> &command_parts);

	/**
	* Loads the file from the hard drive where the commands are prepared and starts doing them sequentially. The format is one command per line.
	* @param command Parts of a command.
	*/
	void load(const vector<string> &command_parts);

	/**
	* Shows help.
	* @param command Parts of a command.
	*/
	void help(const vector<string> &command_parts);

	/**
	* Exits the application.
	* @param command Parts of a command.
	*/
	void quit(const vector<string> &command_parts);
};

#endif // !LINE_COMMANDER_H
