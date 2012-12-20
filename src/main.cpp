/*!
 * \brief  Базовая реализация запуска плагинов
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   12.10.2012
 */

// порядок включений 0 - clib; 1 - stl; 2 - boost; 3 - cpp; 4 - заголовочный данного класса


#include <iostream>
#include <string>
#include <cstring>

#include "Plugin.hpp"


class Config
    : public Plugin
{
    //! \brief Обязательный метод любого наследника плагина.
    bool execute() {
        return true;
    }

public:
    /*!
     * \brief Конструктор инициализации класса управления плагинами
     * \param argc Количество аргументов
     * \param argv Массив аргументов
     * \param env Массив переменных окружения
     */
    Config(int argc, char **argv, char **env) {
        Plugin::_argc = argc;
        Plugin::_argv = argv;
        Plugin::_env  = env;

        std::clog << "Args: ";

        for (int i = 1; i not_eq argc; i++) {
            std::clog << argv[i] << " ";
        }
        std::clog << std::endl;
    }

    //! \brief Метод получения текущего количества загруженных плагинов.
    std::size_t size() {
        return Plugin::size();
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////////


/*!
 * \brief
 *  Метод возвращает путь к загружаемому плагину или локальный путь.
 */
std::string WorkPath(int argc, char **argv) {
    if ((argc > 1) and (argv[1][0] not_eq '-')) {
        return argv[1];
    }
    return ".";
}


/*!
 * \brief
 *  Программа ищет в локальной директори плагины и по очереди их запускает,
 *  передаёт плагинам входные параметры.
 */
int main(int argc, char **argv, char **env) {
    std::clog << "Start run application.\n" << std::flush;

    Config parent(argc, argv, env);

    std::clog << "Loading...\n";
    parent.load(WorkPath(argc, argv));

    if (not parent.size()) {
        std::cerr << "Can`t load libraryes.\n" << std::flush;
    }
    else {
        parent.close();
    }

    std::clog << "Stop run application.\n" << std::flush;
    return 0;
}
