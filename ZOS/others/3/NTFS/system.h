#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>
#include "file_system.h"

using namespace std;

class System {
public:
	/**
	* Returns the instance of a system.
	* @return Instance of a system.
	*/
	static System &getSystem();

	/**
	* Creates a new file system with the given name and change actual path.
	* @param name The name of the file system.
	*/
	void mountNewFileSystem(string name);

	/** 
	* Returns a file system with the given name.
	* @param name The given name of the file system.
	* @return Found file system or NULL.
	*/
	FileSystem* getFileSystem(string name = "");

private:
	vector<FileSystem*> file_system_vector;

	/**
	* Creates a new system.
	*/
	System() {}

	/**
	* Overriden copy constructor to prevent creation of new instances.
	* @param Reference to other system.
	*/
	System(System const&);

	/**
	* Overriden simple assignment to prevent changes of the instance.
	* @param Reference to other system.
	* @return Reference to new system.
	*/
	System& operator=(System const&);

	/**
	* Destructs a system.
	*/
	~System();
};

#endif // !SYSTEM_H