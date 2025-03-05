#include "helpers.h"

// trim leading and trailing white space from char array
void trim_ws(char str[]) {
    if (str == nullptr || str[0] == '\0') {
        return;
    }

    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r') {
        start++;
    }

    int end = strlen(str) - 1;
    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r')) {
        end--;
    }

    int length = end - start + 1;
    std::memmove(str, str + start, length);
    str[length] = '\0';
}


// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

// trim from start (copying)
inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}




