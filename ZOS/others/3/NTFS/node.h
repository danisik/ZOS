#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <string>
#include <vector>
#include "path.h"

using namespace std;

class Node {
public:
	/** Size of a node. */
	int size;
	/** Name of a node. */
	const string name;
	/** Path of a node. */
	const Path path;
	/** Is directory flag. */
	const bool is_directory;

	/** 
	* Creates a node with the given path and directory flag.
	* @param path The given path.
	* @param is_directory The directory flag.
	*/
	Node(Path path, bool is_directory);

	/** 
	* Adds addresses of data.
	* @param address New address.
	*/
	void addData(uint32_t address);

	/** 
	* Returns addresses of data.
	* @return Addresses.
	*/
	const vector<uint32_t>& getData() const;
private:
	/** Vector of addresses. */
	vector<uint32_t> data_address;
};

#endif // !NODE_H



