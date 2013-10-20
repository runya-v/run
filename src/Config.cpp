#include "Config.hpp"

using namespace base;


Config::Config(ConfigSerializatorPtr &serializator)
    :_serializator(serializator)
{}


Config::~Config()
{}


void Config::load(const std::string &path) {
    _file_name = path;
    reset(_serializator->parse(path));
}


void Config::save() {
    _serializator->save(_file_name, _storage);
}


void Config::save(const std::string &path) {
    _serializator->save(path, _storage);
}
