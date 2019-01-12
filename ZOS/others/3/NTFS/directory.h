#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "node.h"
#include "path.h"
#include "file_system.h"

class Directory {
public:
	/**
	* Creates a new directory with the given path.
	* @param path The given path.
	*/
	Directory(Path path);

	/**
	* Opens a directory in two different modes. "w" opens directory for writing and 
	* "r" for reading.
	* @param mode The mode.
	*/
	void open(string mode);

	/**
	* Reads the content of a directory as a string.
	* @return The content of the directory
	*/
	string read();

	/**
	* Deletes a directory.
	*/
	void remove();
	
	/**
	* Returns info about a directory.
	* @param Info about the directory.
	*/
	string info() const;

	/**
	* Returns path of a directory.
	* @return The path of the file.
	*/
	const Path& getPath() const;

private:
	/** Node representation of a file used for communication with a file system. */
	Node node;
	/** Path of a file. */
	Path path;
	/** File system indicating where the file is saved. */
	FileSystem *file_system;
	/** Opened flag. */
	bool opened;
};

#endif // !DIRECTORY_H



