#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__CYGWIN__)
#include <pthread.h>
#endif	

#include <cstdio>
#include <stdint.h>
#include <string>
#include <vector>
#include "path.h"
#include "node.h"

using namespace std;

class FileSystem {
public:	
	/** Name of a file system. */
	const string name;

	/**
	* Creates a new file system with the given name.
	*/
	FileSystem(const string name);

	/**
	* Destructs a file system and releases all its resources.
	*/
	~FileSystem();

	/**
	* Checks a path validity. Function uses generateNodeID to find file on a file system.
	* @param path The path.
	* @return true if path exists and is valid.
	*/
	virtual bool isPathValid(const Path &path) const = 0;

	/**
	* Allocates a new node on a file system.
	* @param node The new allocated node.
	*/
	virtual void allocateNode(const Node &node) = 0;

	/**
	* Opens a node and set its members. If node does not exist it throws file_not_found exception.
	* @param node The node from a file system.
	*/
	virtual void openNode(Node &node) = 0;

	/**
	* Opens a node and set its members.
	* @param uid The uid of a node from a file system.
	* @param parent Path to a parent of the node.
	* @return Found node or null.
	*/
	virtual Node* openNode(const uint32_t uid, const Path &parent) const = 0;

	/**
	* Reads data from the given address on a file system. The given vector is cleared before reading.
	* @param address The address of data. wrong_address is thrown when the address does not point to a data section.
	* @param s_data Smallest data block that was read.
	*/
	virtual void readData(const uint32_t address, vector<unsigned char> &s_data) const = 0;

	/**
	* Writes an existing node into a file system. Second parameter contains data that can be stored on a file system.
	* If node has not been alocated function throws alloc_error.
	* @param node The node that is going to be written.
	* @param data New data holded and stored by the given node.
	*/
	virtual void writeNode(Node &node, const vector<unsigned char> &data = vector<unsigned char>()) = 0;

	/**
	* Deletes the given node from a file system. Function does not remove data from a file system, just its pointers.
	* If node has not been alocated function throws alloc_error. If the node is a directory with children function throws
	* delete_error.
	* @param node The given node that is going to be deleted.
	*/
	virtual void deleteNode(const Node &node) = 0;

	/**
	* Copies the given node to the selected path. If node has not been alocated function throws alloc_error.
	* If the target path does not exist or is not a directory function throws path_not_found.
	* @param node The given node that is going to be copied.
	* @param path The target path of a copy process.
	*/
	virtual void copyNode(const Node &node, const Path &path) = 0;

	/**
	* Moves the given node to the selected path. If node has not been alocated function throws alloc_error.
	* If the target path does not exist or is not a directory function throws path_not_found.
	* @param node The given node that is going to be moved.
	* @param path The target path of a move process.
	*/
	virtual void moveNode(const Node &node, const Path &path) = 0;

	/**
	* Data block of files will be placed in one fragment. No spaces between data blocks and files are allowed.
	*/
	virtual void defragment() = 0;

	/**
	* Checks size of files and number of data segments per file. If file system is corrupted function throws
	* consistency_error.
	*/
	virtual void checkConsistency() = 0;

protected:
	/** Number of threads. */
	static const int NUM_THREADS = 10;
	/** Name of a file that stores a whole file system. */
	const string file_name;
	/** Pointer to a file that stores a whole file system used for reading and writing. */
	FILE *file;
	/** Protection of critical sections. */
#if defined(_WIN32)
	CRITICAL_SECTION critical_section;
	HANDLE threads[NUM_THREADS];
#elif defined(__unix__) || defined(__CYGWIN__)
	pthread_mutex_t mutex;
	pthread_t threads[NUM_THREADS];
#endif	

	/**
	* Writes data into the given address on a file system. It can write just one block of data. Number of written bytes is returned from a function.
	* @param address The address of data. Invalid argument is thrown when the address does not point to a data section.
	* @param s_data Data that is going to be written.
	* @return The number of written bytes.
	*/
	virtual uint32_t writeData(const uint32_t address, const vector<unsigned char> &s_data) = 0;

	/**
	* Generates a unique uid from the given path. Function should use some hash function.
	* @param path The given path which is going to be transformed to the uid.
	* @return The unique 32-bit long uid.
	*/
	virtual uint32_t generateNodeID(const string path) const = 0;
};

#endif // !FILE_SYSTEM_H
