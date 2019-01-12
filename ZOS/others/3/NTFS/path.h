#ifndef PATH_H
#define PATH_H

#include <string>
#include <vector>

using namespace std;

class Path {
public:
	/**
	* Creates a new absolute path. The given string has to represent absolute path.
	* @param path String representation of the path.
	*/
	Path(string path);

	/**
	* Creates a new absolute path.
	* @param path String representation of the path.
	*/
	Path(string path, string current_path);

	/**
	* Converts path to string.
	* @return The converted path.
	*/
	string toString() const;

	/**
	* Returns the root part of a path.
	* @return The root..
	*/
	string getRoot() const;

	/**
	* Returns a vector of path parts.
	* @return The vector.
	*/
	const vector<string>& getPathParts() const;

private:
	/** Parts of the path. */
	vector<string> path_parts;

	/**
	* Prepends the given path before this one.
	* @param path The path that is going to be prepended.
	*/
	void prepend(string path);

	/**
	* Appends the given path after this one.
	* @param path The path that is going to be appended.
	*/
	void append(string path);
};

#endif // !PATH_H


