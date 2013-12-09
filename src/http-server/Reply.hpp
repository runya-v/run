/*!
 * \brief  ASIO сервер обслуживающий TCP соединения.
 * \author Rostislav Velichko.
 * \date   02.03.2013
 */

#pragma once


#include <string>
#include <vector>
#include <boost/asio.hpp>

#include "base.hpp"
#include "Request.hpp"


namespace http_server {

    namespace asio = boost::asio;


    /// A reply to be sent to a client.
    struct Reply {
        typedef asio::const_buffer Buffer;
        typedef std::vector<Buffer> Buffers;

        /// The status of the reply.
        enum StatusType {
            ok                    = 200,
            created               = 201,
            accepted              = 202,
            no_content            = 204,
            multiple_choices      = 300,
            moved_permanently     = 301,
            moved_temporarily     = 302,
            not_modified          = 304,
            bad_request           = 400,
            unauthorized          = 401,
            forbidden             = 403,
            not_found             = 404,
            internal_server_error = 500,
            not_implemented       = 501,
            bad_gateway           = 502,
            service_unavailable   = 503
        } _status;

      /// The headers to be included in the reply.
      Headers _headers;

      /// The content to be sent in the reply.
      std::string _content;

      /// Convert the reply into a vector of buffers. The buffers do not own the
      /// underlying memory blocks, therefore the reply object must remain valid and
      /// not be changed until the write operation has completed.
      Buffers toBuffers();

      /// Get a stock reply.
      static Reply stockReply(StatusType status);
    };
} // namespace http_server
