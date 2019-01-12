#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

using namespace std;

/**
* Defines a new exception that is thrown .
*/
class path_not_found : public exception {
public:
	/**
	* Creates a new exception that is thrown .
	* @param what Description of the exception.
	*/
	path_not_found(string what) : message(what) {}

	~path_not_found() throw () {}

	/**
	* Returns description of the exception.
	* @return Description.
	*/
	const char* what() const throw() {
		return this->message.data();
	}

private:
	/** Description of an exception. */
	string message;
};

/**
* Defines a new exception that is thrown .
*/
class file_not_found : public exception {
public:
	/**
	* Creates a new exception that is thrown .
	* @param what Description of the exception.
	*/
	file_not_found(string what) : message(what) {}

	~file_not_found() throw () {}

	/**
	* Returns description of the exception.
	* @return Description.
	*/
	const char* what() const throw() {
		return this->message.data();
	}

private:
	/** Description of an exception. */
	string message;
};

/**
* Defines a new exception that is thrown .
*/
class wrong_address : public exception {
public:
	/**
	* Creates a new exception that is thrown .
	* @param what Description of the exception.
	*/
	wrong_address(string what) : message(what) {}

	~wrong_address() throw () {}

	/**
	* Returns description of the exception.
	* @return Description.
	*/
	const char* what() const throw() {
		return this->message.data();
	}

private:
	/** Description of an exception. */
	string message;
};

/**
* Defines a new exception that is thrown .
*/
class alloc_error : public exception {
public:
	/**
	* Creates a new exception that is thrown .
	* @param what Description of the exception.
	*/
	alloc_error(string what) : message(what) {}

	~alloc_error() throw () {}

	/**
	* Returns description of the exception.
	* @return Description.
	*/
	const char* what() const throw() {
		return this->message.data();
	}

private:
	/** Description of an exception. */
	string message;
};

/**
* Defines a new exception that is thrown .
*/
class delete_error : public exception {
public:
	/**
	* Creates a new exception that is thrown .
	* @param what Description of the exception.
	*/
	delete_error(string what) : message(what) {}

	~delete_error() throw () {}

	/**
	* Returns description of the exception.
	* @return Description.
	*/
	const char* what() const throw() {
		return this->message.data();
	}

private:
	/** Description of an exception. */
	string message;
};

/**
* Defines a new exception that is thrown .
*/
class uid_error : public exception {
public:
	/**
	* Creates a new exception that is thrown .
	* @param what Description of the exception.
	*/
	uid_error(string what) : message(what) {}

	~uid_error() throw () {}

	/**
	* Returns description of the exception.
	* @return Description.
	*/
	const char* what() const throw() {
		return this->message.data();
	}

private:
	/** Description of an exception. */
	string message;
};

/**
* Defines a new exception that is thrown .
*/
class no_space : public exception {
public:
	/**
	* Creates a new exception that is thrown .
	* @param what Description of the exception.
	*/
	no_space(string what) : message(what) {}

	~no_space() throw () {}

	/**
	* Returns description of the exception.
	* @return Description.
	*/
	const char* what() const throw() {
		return this->message.data();
	}

private:
	/** Description of an exception. */
	string message;
};

/**
* Defines a new exception that is thrown .
*/
class consistency_error : public exception {
public:
	/**
	* Creates a new exception that is thrown .
	* @param what Description of the exception.
	*/
	consistency_error(string what) : message(what) {}

	~consistency_error() throw () {}

	/**
	* Returns description of the exception.
	* @return Description.
	*/
	const char* what() const throw() {
		return this->message.data();
	}

private:
	/** Description of an exception. */
	string message;
};

#endif // !EXCEPTION_H
