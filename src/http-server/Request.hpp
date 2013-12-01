/*!
 * \brief  Структура ответа клиенту.
 * \author Rostislav Velichko.
 * \date   02.03.2013
 */

#pragma once


#include <string>
#include <vector>

#include "Header.hpp"


namespace http_server {

    typedef std::vector<Header> Headers;


    struct Request {
        std::string _method;
        std::string _uri;
        int _http_version_major;
        int _http_version_minor;
        Headers _headers;
    };
} // namespace http_server
