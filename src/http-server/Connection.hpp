/*!
 * \brief  ASIO сервер обслуживающий TCP соединения.
 * \author Rostislav Velichko.
 * \date   02.03.2013
 */

#pragma once


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "Reply.hpp"
#include "Request.hpp"
#include "RequestHandler.hpp"
#include "RequestParser.hpp"


namespace http_server {

    /// Represents a single connection from a client.
    class connection
        : public boost::enable_shared_from_this< Connection >
        , private boost::noncopyable
    {
        /// Strand to ensure the connection's handlers are not called concurrently.
        boost::asio::io_service::strand strand_;

        /// Socket for the connection.
        boost::asio::ip::tcp::socket socket_;

        /// The handler used to process the incoming request.
        RequestHandler &request_handler_;

        /// Buffer for incoming data.
        boost::array< char, 8192 > buffer_;

        /// The incoming request.
        Request request_;

        /// The parser for the incoming request.
        RequestParser request_parser_;

        /// The reply to be sent back to the client.
        Reply reply_;

        /// Handle completion of a read operation.
        void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred);

        /// Handle completion of a write operation.
        void handleWrite(const boost::system::error_code& e);

    public:
        /// Construct a connection with the given io_service.
        explicit connection(boost::asio::io_service& io_service, request_handler& handler);

        /// Get the socket associated with the connection.
        boost::asio::ip::tcp::socket& socket();

        /// Start the first asynchronous operation for the connection.
        void start();
    };

    typedef boost::shared_ptr< Connection > PConnection;
} // namespace http_server
