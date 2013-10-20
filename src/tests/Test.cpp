/*!
 * \brief  Типовой пример базового класса плагина.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   15.10.2012
 */


#include <iostream>

#include <boost/filesystem.hpp>

#include "Plugin.hpp"


class Test
    : public Plugin
{
public:
    //! \brief Вызывается при получении указателя на данный объект.
    Test() {
    }

    //! \brief Метод вызывается после инициализации плагина.
    bool execute() {
        std::cout << "Test::execute:"   << std::endl;
        std::cout << "name: `" << Plugin::_name << "`" << std::endl;

        int i;

        for (int i = 0; i < Plugin::_argc; ++i) {
            std::cout << "argv[" << i << "]: " << _argv[0] << std::endl;
        }

        i = 0;

        while (_env[i]) {
            std::cout << "env[" << i << "]: " << Plugin::_env[i] << std::endl;
            i++;
        }

        std::cout << std::endl;



        /*if (Plugin::_argc > 2) {
            std::string lib = _argv[1];

            Plugin::_argc--;
            Plugin::_argv++;
            Plugin *plugin = Plugin::get(lib);

            if (plugin) {
                return plugin->execute();
            }
            return false;
        }*/
        return true;
    }
} test; //! \brief Обязательная глобальная переменная объекта плагина.

