
#include "utility.h"

/**
* Splits a string to parts seperated by tokens.
* @param s The string.
* @param tokens Tokens.
* @param s_parts Parts of the splitted string.
*/
void split(string s, const string tokens, vector<string> &s_parts) {
	string::const_iterator begin = s.begin();
	const string::const_iterator end = s.end();

	for (string::const_iterator it = begin; it != end; it++) {
		if (tokens.find(*it) != string::npos) {
			if (begin != it) {
				s_parts.push_back(string(begin, it));
			}

			begin = it + 1;
		}
	}

	if (begin != end) {
		s_parts.push_back(string(begin, end));
	}
}

/**
* Trims from end of string (right).
* @param s The string that is going to be trimmed.
* @param t The trimmed characters.
*/
string rtrim(string s, const string t) {
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

/**
* Trims from beginning of string (left).
* @param s The string that is going to be trimmed.
* @param t The trimmed characters.
*/
string ltrim(string s, const string t) {
	s.erase(0, s.find_first_not_of(t));
	return s;
}

/**
* Trims from both ends of string (left and right).
* @param s The string that is going to be trimmed.
* @param t The trimmed characters.
*/
string trim(string s, const string t) {
	return ltrim(rtrim(s, t), t);
}