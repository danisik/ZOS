#include <stdexcept>
#include "commander.h"
#include "file_system.h"
#include "system.h"

/*
* __________________________________CONSTRUCTORS______________________________________
*/

/**
* Creates a new commander and sets its directory.
*/
Commander::Commander() {
	FileSystem *file_system = System::getSystem().getFileSystem();
	if (file_system == NULL) {
		throw runtime_error("COMMANDER CONSTRUCTOR: Cannot start commander: Any file systems have not been mounted.");
	}

	this->act_dir = '/' + file_system->name;
}

/*
* __________________________________PUBLIC FUNCTIONS______________________________________
*/

/**
* Returns an actual directory.
* @return The actual directory.
*/
const string Commander::getActualDir() const {
	return this->act_dir;
}

/**
* Sets an actual directory.
* @param path The actual directory.
*/
void Commander::setActualDir(string path) {
	this->act_dir = path;
}