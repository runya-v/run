/*!
 * \brief  ASIO сервер обслуживающий TCP соединения.
 * \author Rostislav Velichko.
 * \date   02.03.2013
 */

#pragma once

#include <string>


namespace http_server {
    namespace mime_types {
        /// Convert a file extension into a MIME type.
        std::string ExtensionToType(const std::string& extension);
    } // namespace mime_types
} // namespace http_server
