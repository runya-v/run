/*!
 * \brief  ASIO сервер обслуживающий TCP соединения.
 * \author Rostislav Velichko.
 * \date   02.03.2013
 */

#pragma once

#include <string>


namespace http_server {

    struct header {
        std::string name;
        std::string value;
    };
} // namespace http_server
