#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <queue>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

#include <limits.h>
#include <stdio.h>
#include <limits.h>

#include <Wt/WString>

#include "UnCript.hpp"


using namespace web;


UnCript::UnCript(const std::string& key, const std::string& in_text)
    : _key(key)
    , _in_text(in_text)
{
    std::ifstream ifs_key(key);

    if (ifs_key.is_open()) {
        _res << "<p>key:</p>";
        _res << "<p>";

        for (std::string line; getline(ifs_key, line);) {
            _res << line;
        }
        _res << "</p><br/>";

        std::ifstream ifs_text(in_text);

        if (ifs_text.is_open()) {
            _res << "<p>text:</p>";
            _res << "<p>";

            for (std::string line; getline(ifs_text, line);) {
                _res << line;
            }
            _res << "</p>";
        }
    }
}


UnCript::operator std::string() {
    return _res.str();
}
