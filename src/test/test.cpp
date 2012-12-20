/*!
 * \brief  Типовой пример базового класса плагина.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   15.10.2012
 */

#include <iostream>

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
        std::cout << "argc: "  << Plugin::_argc << std::endl;
        std::cout << "argv: "  << Plugin::_argv[0] << std::endl;
        std::cout << "env: "   << Plugin::_env[0]  << std::endl << std::endl;
        return true;
    }
} test; //! \brief Обязательная глобальная переменная объекта плагина.

