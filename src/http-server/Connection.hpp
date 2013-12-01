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
    class Connection
        : public boost::enable_shared_from_this< Connection >
        , private boost::noncopyable
    {
        /// Strand to ensure the connection's handlers are not called concurrently.
        boost::asio::io_service::strand _strand;

        /// Socket for the connection.
        boost::asio::ip::tcp::socket _socket;

        /// The handler used to process the incoming request.
        RequestHandler &_request_handler;

        /// Buffer for incoming data.
        boost::array<char, 8192> _buffer;

        /// The incoming request.
        Request _request;

        /// The parser for the incoming request.
        RequestParser _request_parser;

        /// The reply to be sent back to the client.
        Reply _reply;

        /// Handle completion of a read operation.
        void handleRead(const boost::system::error_code& e, std::size_t bytes_transferred);

        /// Handle completion of a write operation.
        void handleWrite(const boost::system::error_code& e);

    public:
        /// Construct a connection with the given io_service.
        explicit Connection(boost::asio::io_service& io_service, RequestHandler& handler);

        /// Get the socket associated with the connection.
        boost::asio::ip::tcp::socket& socket();

        /// Start the first asynchronous operation for the connection.
        void start();
    };

    typedef boost::shared_ptr<Connection> PConnection;
} // namespace http_server
