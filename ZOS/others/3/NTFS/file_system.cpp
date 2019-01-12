#include "file_system.h"

/*
* __________________________________CONSTRUCTORS______________________________________
*/

/**
* Creates a new file system with the given name.
*/
FileSystem::FileSystem(const string name) : name(name), file_name(name + ".dat") {
#if defined(_WIN32)
	InitializeCriticalSection(&this->critical_section);
#elif defined(__unix__) || defined(__CYGWIN__)
	this->mutex = PTHREAD_MUTEX_INITIALIZER;
#endif	
}

/**
* Destructs a file system and sets all resources free.
*/
FileSystem::~FileSystem() { 
	fclose(this->file); 
}