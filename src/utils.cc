#include "utils.hpp"
#include <cmath>
#include <algorithm>
#include <stdio.h>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        if (!item.empty())
            *(result++) = item;
    }
};

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
};

double parse_float (std::string str) {
    std::string number = "0";
    std::string ext = "";
    bool is_float = false;
    bool is_number = true;

    for (auto & c : str) {
        if (c == '.' || c == ',') {
            if (is_float) is_number = false;
            is_float = true;
            number.push_back('.');
            continue;
        }

        if (is_number) {
            if (isdigit(c)) {
                number.push_back(c);
            } else {
                is_number = false;
                ext.push_back(c);
            }
        } else {
            ext.push_back(c);
        }
    }

    double f = strtod(number.c_str(), NULL);

    std::vector<std::vector<std::string>> bases = {
        { "k", "thousand" },
        { "mil", "m", "million" },
        { "b", "bil", "billion" }
    };

    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c){ return std::tolower(c); });

    for (size_t x = 0; x < bases.size(); x++) {
        for (auto & e : bases[x]) {
            if (e == ext) {
                return f * pow(10, ((x + 1) * 3));
            }
        }
    }

    return f;
}