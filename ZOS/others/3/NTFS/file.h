#ifndef FILE_H
#define FILE_H

#include <string>
#include <vector>
#include "path.h"
#include "node.h"
#include "file_system.h"

using namespace std;

class File {
public:
	/**
	* Creates a new file with the given path.
	* @param path The given path.
	*/
	File(Path path);

	/**
	* Opens a file in two different modes. "w" opens file for writing and "r" for reading.
	* @param mode The mode.
	*/
	void open(string mode);

	/**
	* Writes the given string into a file.
	* @param The given string.
	*/
	void write(string data);

	/**
	* Writes the given vector of bytes into a file.
	* @param The given vector of bytes.
	*/
	void write(const vector<unsigned char> &data);

	/**
	* Reads the content of a file as a string.
	* @return The content of the file
	*/
	string read() const;

	/**
	* Reads the content of a file as a byte vector.
	* @param content The content of the file.
	*/
	void readByte(vector<unsigned char>& content) const;

	/**
	* Copies file to the given directory.
	* @param The target directory.
	*/
	void copy(const Path &path);

	/**
	* Moves file to the given directory.
	* @param The target directory.
	*/
	void move(const Path &path);

	/**
	* Deletes a file.
	*/
	void remove();

	/** 
	* Returns info about a file.
	* @param Info about the file.
	*/
	string info() const;

	/** 
	* Returns path of a file.
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

#endif // !FILE_H





