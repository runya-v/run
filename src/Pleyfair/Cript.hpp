#pragma once


#include <sstream>
#include <string>
#include <vector>

#include "Pleyfair.hpp"


namespace web {

    class Cript {
        std::string _key;
        std::string _in_text;
        std::stringstream _res;

        bool checkKey(const std::string &key);
        std::string make(const std::string &key, const std::string &text);

    public:
        Cript(const std::string &key, const std::string &in_text);

        operator std::string();
    };
}
