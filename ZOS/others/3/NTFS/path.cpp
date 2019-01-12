#include <iostream>
#include <stdexcept>
#include "path.h"
#include "utility.h"

/*
* __________________________________CONSTRUCTORS______________________________________
*/

/**
* Creates a new absolute path. The given string has to represent absolute path.
* @param path String representation of the path.
*/
Path::Path(string path) {
	if (path.empty()) {
		throw invalid_argument("PATH CONSTRUCTOR: The given path cannot be empty.");
	}
	
	if (path.at(0) != '/') {
		throw invalid_argument("PATH CONSTRUCTOR: The given path is not absolute.");
	}

	split(path, "/", this->path_parts);
}

/**
* Creates a new absolute path.
* @param path String representation of the path.
*/
Path::Path(string path, string current_path) {
	if (path.empty()) {
		throw invalid_argument("PATH CONSTRUCTOR: The given path cannot be empty.");
	}

	split(path, "/", this->path_parts);
	if (path.at(0) != '/') {
		this->prepend(current_path);
	}
}

/*
* __________________________________PUBLIC FUNCTIONS______________________________________
*/

/**
* Converts path to string.
* @return The converted path.
*/
string Path::toString() const{
	string path;
	for (vector<string>::const_iterator it = this->path_parts.begin(); it != this->path_parts.end(); it++) {
		path += '/' + *it;
	}

	return path;
}

/**
* Returns the root part of a path.
* @return The root..
*/
string Path::getRoot() const {
	return this->path_parts.front();
}

/**
* Returns a vector of path parts.
* @return The vector.
*/
const vector<string>& Path::getPathParts() const {
	return this->path_parts;
}

/*
* __________________________________PRIVATE FUNCTIONS______________________________________
*/

/**
* Prepends the given path before this one.
* @param path The path that is going to be prepended.
*/
void Path::prepend(string path) {
	vector<string> temp_parts;
	split(path, "/", temp_parts);
	this->path_parts.insert(this->path_parts.begin(), temp_parts.begin(), temp_parts.end());
}

/**
* Appends the given path after this one.
* @param path The path that is going to be appended.
*/
void Path::append(string path) {
	vector<string> temp_parts;
	split(path, "/", temp_parts);
	this->path_parts.insert(this->path_parts.end(), temp_parts.begin(), temp_parts.end());
}