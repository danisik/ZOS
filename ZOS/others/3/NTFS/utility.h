#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>

using namespace std;

/**
* Splits a string to parts seperated by tokens.
* @param s The string.
* @param tokens Tokens.
* @param s_parts Parts of the splitted string.
*/
void split(string s, const string tokens, vector<string> &s_parts);

const string white_spaces = " \t\n\r\f\v";

/**
* Trims from end of string (right).
* @param s The string that is going to be trimmed.
* @param t The trimmed characters.
*/
string rtrim(string s, const string t = white_spaces);

/**
* Trims from beginning of string (left).
* @param s The string that is going to be trimmed.
* @param t The trimmed characters.
*/
string ltrim(string s, const string t = white_spaces);

/**
* Trims from both ends of string (left and right).
* @param s The string that is going to be trimmed.
* @param t The trimmed characters.
*/
string trim(string s, const string t = white_spaces);


#endif // !UTILITY_H
