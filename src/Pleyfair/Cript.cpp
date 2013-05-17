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

#include "Cript.hpp"


using namespace web;


std::string Cript::make(const std::string &key, const std::string &text) {

}


Cript::Cript(const std::string &key, const std::string &in_text)
    : _key(key)
    , _in_text(in_text)
{
    std::ifstream ifs_key(key);

    if (ifs_key.is_open()) {
        std::string key;

        for (std::string line; getline(ifs_key, line);) {
            key += line;
        }

        _res << "<p>key:</p>";
        _res << "<p>" << key << "</p><br/>";

        std::ifstream ifs_text(in_text);

        if (ifs_text.is_open()) {
            std::string text;

            for (std::string line; getline(ifs_text, line);) {
                text += line;
            }

            _res << "<p>text:</p>";
            _res << "<p>" << text << "</p>";

            _res << "<p>result:</p>";
            _res << "<p>" << make(key, text) << "</p>";
        }
    }
}


Cript::operator std::string() {
    return _res.str();
}
