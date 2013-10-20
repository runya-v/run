#pragma once

#include "ConfigSerializator.hpp"
#include "ConfigDepot.hpp"


namespace base {
    
    class Config
        : public ConfigDepot
    {
        ConfigSerializatorPtr _serializator;
        std::string _file_name;

    public:
        Config(ConfigSerializatorPtr &serializator);
        ~Config();

        Config(const Config&) = delete;
        Config & operator= (const Config&) = delete;

        void load(const std::string &path);
        void save();
        void save(const std::string &path);
    };
} // namespace base
