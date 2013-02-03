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

    typedef std::vector< Header > Headers;


struct Request {
      std::string method;
      std::string uri;
      int http_version_major;
      int http_version_minor;
      Headers headers;
    };
} // namespace http_server
