#pragma once

#include <algorithm> 
#include <cstring>
#include <cctype>
#include <locale>

typedef enum { ALPHANUM, POSITION  } namingstate_t;
typedef enum { NONE, SINGLE_CLICK, DOUBLE_CLICK, LONG_PRESS  } selection_t;


void trim_ws(char str[]);

// trim from start (in place)
void ltrim(std::string &s);

// trim from end (in place)
void rtrim(std::string &s);

// trim from both ends (in place)
void trim(std::string &s);

// trim from start (copying)
std::string ltrim_copy(std::string s);

// trim from end (copying)
std::string rtrim_copy(std::string s);

// trim from both ends (copying)
std::string trim_copy(std::string s);

template <typename T> T wrap(T value, T min, T max) ;

