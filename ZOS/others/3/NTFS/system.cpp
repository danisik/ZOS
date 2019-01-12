#include <iostream>
#include "system.h"
#include "ntfs.h"

/*
* __________________________________PUBLIC FUNCTIONS______________________________________
*/

/**
* Returns the instance of a system.
* @return Instance of a system.
*/
System& System::getSystem() {
	static System instance;
	return instance;
}

/**
* Creates a new file system with the given name and change actual path.
* @param name The name of the file system.
*/
void System::mountNewFileSystem(string name) {
	NTFS* file_system = new NTFS(name);
	file_system->checkConsistency();
	this->file_system_vector.push_back(file_system);
}

/**
* Returns a file system with the given name.
* @param name The given name of the file system.
* @return Found file system or NULL.
*/
FileSystem* System::getFileSystem(string name) {
	if (name.empty() && !this->file_system_vector.empty()) {
		return this->file_system_vector.front();
	}

	for (vector<FileSystem*>::const_iterator it = this->file_system_vector.begin(); it != this->file_system_vector.end(); it++) {
		if ((*it)->name.compare(name) == 0) {
			return *it;
		}
	}

	return NULL;
}

/*
* __________________________________PRIVATE FUNCTIONS______________________________________
*/

/**
* Destructs a system.
*/
System::~System() {
	for (vector<FileSystem*>::iterator it = this->file_system_vector.begin();
		it != this->file_system_vector.end(); it++) {
		delete *it;
	}
}