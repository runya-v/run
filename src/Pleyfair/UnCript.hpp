#pragma once


#include <sstream>
#include <string>
#include <vector>


namespace web {

    class UnCript {
        std::string _key;
        std::string _in_text;
        std::stringstream _res;

    public:
        UnCript(const std::string &key, const std::string &in_text);

        operator std::string();
    };
}
