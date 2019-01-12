
#include <stdexcept>
#include <sstream>
#include "system.h"
#include "file.h"
#include "exception.h"

/**
* Creates a new file with the given path.
* @param path The given path.
*/
File::File(Path path) : path(path), file_system(System::getSystem().getFileSystem(this->path.getRoot())), node(path, false), opened(false) {
	if (this->file_system == NULL) {
		throw file_not_found("FILE CONSTRUCTOR: The entered path does not exist.");
	}

	if (this->node.name.size() > 12) {
		throw out_of_range("FILE CONSTRUCTOR: The entered name is too long.");
	}
}

/**
* Opens a file in two different modes. "w" opens file for writing and "r" for reading.
* @param mode The mode.
*/
void File::open(string mode) {
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
* Writes the given string into a file.
* @param The given string.
*/
void File::write(string data) {
	if (opened) {
		vector<unsigned char> content(data.begin(), data.end());
		this->file_system->writeNode(this->node, content);
	}
}

/**
* Writes the given vector of bytes into a file.
* @param The given vector of bytes.
*/
void File::write(const vector<unsigned char> &data) {
	if (opened) {
		this->file_system->writeNode(this->node, data);
	}
}

/**
* Reads the content of a file as a string.
* @return The content of the file
*/
string File::read() const {
	if (opened) {
		vector<unsigned char> content;
		for (vector<uint32_t>::const_iterator it = this->node.getData().begin(); it != this->node.getData().end(); it++) {
			vector<unsigned char> temp_content;
			this->file_system->readData(*it, temp_content);
			content.insert(content.end(), temp_content.begin(), temp_content.end());
		}

		return string(content.begin(), content.end());
	}
}

/**
* Reads the content of a file as a byte vector.
* @param content The content of the file.
*/
void File::readByte(vector<unsigned char>& content) const {
	if (opened) {
		for (vector<uint32_t>::const_iterator it = this->node.getData().begin(); it != this->node.getData().end(); it++) {
			vector<unsigned char> temp_content;
			this->file_system->readData(*it, temp_content);
			content.insert(content.end(), temp_content.begin(), temp_content.end());
		}
	}
}

/**
* Copies file to the given directory.
* @param The target directory.
*/
void File::copy(const Path &path) {
	if (opened) {
		this->file_system->copyNode(this->node, path);
	}
}

/**
* Moves file to the given directory.
* @param The target directory.
*/
void File::move(const Path &path) {
	if (opened) {
		this->file_system->moveNode(this->node, path);
	}
}

/**
* Deletes a file.
*/
void File::remove() {
	if (opened) {
		this->file_system->deleteNode(this->node);
	}
}

/**
* Returns info about a file.
* @param Info about the file.
*/
string File::info() const {
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
* Returns path of a file.
* @return The path of the file.
*/
const Path& File::getPath() const {
	return this->path;
}