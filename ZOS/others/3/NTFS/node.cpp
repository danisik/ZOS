
#include <stdexcept>
#include <iostream>
#include "node.h"
#include "system.h"

/**
* Creates a node with the given path and directory flag.
* @param path The given path.
* @param is_directory The directory flag.
*/
Node::Node(Path path, bool is_directory) : path(path), is_directory(is_directory), name(path.getPathParts().back()) {
}

/**
* Adds addresses of data.
* @param address New address.
*/
void Node::addData(uint32_t address) {
	this->data_address.push_back(address);
}

/**
* Returns addresses of data.
* @return Addresses.
*/
const vector<uint32_t>& Node::getData() const {
	return this->data_address;
}