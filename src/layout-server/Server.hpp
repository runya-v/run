/*!
 * \brief  Класс web сервера на основе вёрстки.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   16.12.2012
 */


#include <string>

#include <cppcms/application.h>


namespace LayoutParser {

    class Server
        : public cppcms::application
    {
    public:
        Server(cppcms::service &srv);
    };
} // LayoutParser
