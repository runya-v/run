/*!
 * \brief  Пример простого web приложения.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   16.10.2012
 */


#include <iostream>
#include <exception>
#include <string>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/fstream.hpp"

#include <cppcms/application.h>
#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>

#include "Plugin.hpp"


class Hw
    : public cppcms::application
{
    std::string _str;

public:
    Hw(cppcms::service &srv)
        : cppcms::application(srv)
    {
        namespace bfs = boost::filesystem;

        std::cout << "Find: `index.html`" << std::endl;

        bfs::ifstream ifs("index.html");

        while (ifs)
        {
            std::string line;
            std::getline(ifs, line);

            if (ifs) {
                _str += line;
            }
        }
    }

    void main(std::string url) {
        std::cout << "Responce url: `" << url << "`" << std::endl;
        response().out() << _str;
    }

};

class HwMain
    : public Plugin
{
public:
    HwMain() {
        std::cout << "Create Hw." << std::endl;
    }

    bool execute() {
        std::cout << "Execute Hw." << std::endl;
        try {
            cppcms::service srv(Plugin::_argc, Plugin::_argv);
            srv.applications_pool().mount(cppcms::applications_factory< Hw >());
            srv.run();
        }
        catch(const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
} hw_main;

