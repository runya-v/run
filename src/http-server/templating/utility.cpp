#include <stdexcept>

#include "Log.hpp"
#include "utility.hpp"

using namespace tmplt;


FileSaver::FileSaver(const char *source, const uint32_t size, const std::string &file_name, bool binary)
    : _path(file_name)
{
    LOG(INFO) << "Create: `" << _path.string() << "`.";
    std::ofstream ofs;
    
    if (binary) {
        ofs.open(_path.string(), std::ios::binary);
    }
    else {
        ofs.open(_path.string());
    }

    if (ofs.is_open()) {
        ofs.write(source, size);
    }
    else {
        throw std::runtime_error("Can`t create file: `" + file_name + "`.");
    }
}


FileSaver::~FileSaver() {
    if (base::bfs::exists(_path)) {
        base::bfs::remove(_path);
        LOG(INFO) << "Delete: `" << _path.string() << "`.";
    }
}

FileSaver::operator const std::string&() const {
    return _path.string();
}


FileSaver::operator const std::string&() {
    return _path.string();
}
////////////////////////////////////////////////////////////////////////////////////////////////////


CFile::CFile(const std::string &file_name, const std::string &options)
    : _file(fopen(file_name.c_str(), options.c_str()))
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
////////////////////////////////////////////////////////////////////////////////////////////////////


StdoutToFile::StdoutToFile(const std::string &file_name) 
    : _saved_stdout(dup(STDOUT_FILENO))
    , _file(fopen(file_name.c_str(), "w"))
{
    if (-1 == _saved_stdout) {
        throw std::runtime_error("Can`t dup stdout.");
    }
    
    if (nullptr == _file) {
        throw std::runtime_error("Can`t open redirect file `" + file_name + "`.");
    }
    
    if (-1 == dup2(fileno(_file), STDOUT_FILENO)) {
        throw std::runtime_error("Can`t redirect to file `" + file_name + "`.");
    }
}

StdoutToFile::~StdoutToFile() {
    fflush(stdout);
    fclose(_file);
    dup2(_saved_stdout, STDOUT_FILENO);
}    
