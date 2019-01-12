
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "directory.h"
#include "system.h"
#include "exception.h"

/**
* Creates a new directory with the given path.
* @param path The given path.
*/
Directory::Directory(Path path) : path(path), file_system(System::getSystem().getFileSystem(this->path.getRoot())), node(path, true), opened(false) {
	if (this->file_system == NULL) {
		throw file_not_found("DIRECTORY CONSTRUCTOR: The entered path does not exist.");
	}

	if (this->node.name.size() > 12) {
		throw out_of_range("DIRECTORY CONSTRUCTOR: The entered name is too long.");
	}
}

/**
* Opens a directory in two different modes. "w" opens directory for writing and
* "r" for reading.
* @param mode The mode.
*/
void Directory::open(string mode) {
	if (mode.compare("r") == 0) {
		this->file_system->openNode(this->node);
	}
	else if (mode.compare("w") == 0) {
		this->file_system->allocateNode(this->node);
		this->file_system->openNode(this->node);
	}
	else {
		throw invalid_argument("Wrong open mode");
	}

	this->opened = true;
}

/**
* Reads the content of a directory as a string.
* @return The content of the directory
*/
string Directory::read() {
	if (opened) {
		vector<unsigned char> content;
		for (vector<uint32_t>::const_iterator it = this->node.getData().begin(); it != this->node.getData().end(); it++) {
			vector<unsigned char> temp_content;
			this->file_system->readData(*it, temp_content);
			content.insert(content.end(), temp_content.begin(), temp_content.end());
		}

		stringstream string_stream;
		string_stream << "DIRECTORY" << setw(15) << "NAME" << setw(13) << "SIZE" << endl;
		for (int i = 0; i < this->node.size; i = i + sizeof(uint32_t)) {
			uint32_t child_uid;
			memcpy(&child_uid, &content.at(i), sizeof(uint32_t));
			Node *node = this->file_system->openNode(child_uid, this->path);
			if (node != NULL) {
				string_stream << setw(9) << node->is_directory << setw(15) << node->name << setw(13) << node->size << endl;
				delete node;
			}
		}
		return string_stream.str();
	}
}

/**
* Deletes a directory.
*/
void Directory::remove() {
	if (opened) {
		this->file_system->deleteNode(this->node);
	}
}

/**
* Returns info about a directory.
* @param Info about the directory.
*/
string Directory::info() const {
	if (opened) {
		stringstream string_stream;
		string_stream << "Name: " << this->node.name << endl
			<< "Size: " << this->node.size << endl
			<< "Is directory: " << this->node.is_directory << endl
			<< "Addresses:" << endl;

		vector<uint32_t> addresses = this->node.getData();
		for (vector<uint32_t>::iterator it = addresses.begin(); it != addresses.end(); it++) {
			string_stream << *it << endl;
		}

		return string_stream.str();
	}
}

/**
* Returns path of a directory.
* @return The path of the file.
*/
const Path& Directory::getPath() const {
	return this->path;
}