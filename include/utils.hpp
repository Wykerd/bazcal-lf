#ifndef BZ_UTILS_HPP
#define BZ_UTILS_HPP
#include <sstream>
#include <string>
#include <vector>

template<typename Out>
void split(const std::string &s, char delim, Out result);

std::vector<std::string> split(const std::string &s, char delim);

double parse_float (std::string str);

#endif