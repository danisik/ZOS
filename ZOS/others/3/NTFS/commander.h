#ifndef COMMANDER_H
#define COMMANDER_H

#include <string>

using namespace std;

class Commander {
public:
	/** 
	* Creates a new commander and sets its directory.
	*/
	Commander();

	/** 
	* Reads a new command from a user.
	*/
	virtual void inputCommand() = 0;

	/**
	* Returns an actual directory.
	* @return The actual directory.
	*/
	const string getActualDir() const;

	/**
	* Sets an actual directory.
	* @param path The actual directory.
	*/
	void setActualDir(string path);

protected:
	/** 
	* Parses a command from a user
	*/
	virtual void parseCommand(string command_input) = 0;

private:
	/** Actual directory. */
	string act_dir;
};

#endif // !COMMANDER_H