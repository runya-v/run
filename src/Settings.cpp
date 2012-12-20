/*!
 * \brief  Плагин настроек
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   18.10.2012
 */


#include <iostream>

#include "Plugin.hpp"


class Settings
    : public Plugin
{
public:
    Settings()
    {
        std::clog << "Settings." << std::endl;
    }

    bool execute() {
        std::clog << "Try load all library in `/home/runya/develop/run/bld/Debug/lib`." << std::endl;
        load("../lib");
        return true;
    }
} settings;
