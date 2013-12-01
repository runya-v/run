/*!
 * \brief  ASIO сервер обслуживающий TCP соединения.
 * \author Rostislav Velichko.
 * \date   02.03.2013
 */

#pragma once

#include <string>
#include <boost/noncopyable.hpp>


namespace http_server {

    struct Reply;
    struct Request;

    /// The common handler for all incoming requests.
    class RequestHandler
        : private boost::noncopyable
    {
        /// The directory containing the files to be served.
        std::string _doc_root;

        /// Perform URL-decoding on a string. Returns false if the encoding was
        /// invalid.
        static bool urlDecode(const std::string& in, std::string& out);

    public:
        /// Construct with a directory containing files to be served.
        explicit RequestHandler(const std::string& doc_root);

        /// Handle a request and produce a reply.
        void handleRequest(const Request &req, Reply &rep);
    };
} // namespace http_server
