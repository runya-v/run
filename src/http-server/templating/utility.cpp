#include <stdexcept>

#include "utility.hpp"

using namespace tmplt;


CFile::CFile(const std::string &file_name)
    : _file(fopen(file_name.c_str(), "w"))
{
    if (nullptr == _file) {
        throw std::runtime_error("Can`t open redirect file `" + file_name + "`.");
    }
}


CFile::~CFile() {
    fclose(_file);
}


CFile::operator FILE* () {
    return _file;
}
