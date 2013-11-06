#include <stdexcept>

#include "Log.hpp"
#include "utility.hpp"

using namespace tmplt;


FileSaver::FileSaver(const char *source, const uint32_t size, const std::string &file_name, bool binary)
    : _path(base::bfs::absolute(base::bfs::current_path()) / file_name)
{
    LOG(METHOD, base::Log::Level::INFO, "Create: `" + _path.string() + "`.");
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
        LOG(METHOD, base::Log::Level::INFO, "Delete: `" + _path.string() + "`.");
    }
}

FileSaver::operator std::string() {
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
