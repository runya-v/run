#include <vector>

#include <boost/bind.hpp>

#include "RequestHandler.hpp"
#include "Connection.hpp"
#include "Log.hpp"


namespace http_server {

    Connection::Connection(boost::asio::io_service& io_service, RequestHandler &handler)
        : _strand(io_service)
        , _socket(io_service)
        , _request_handler(handler)
    {}


    boost::asio::ip::tcp::socket& Connection::socket() {
        return _socket;
    }


    void Connection::start() {
        _socket.async_read_some(
            boost::asio::buffer(_buffer),
            _strand.wrap(boost::bind(
                &Connection::handleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
    }


    void Connection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred) {
        if (not e) {
            boost::tribool result;
            boost::tie(result, boost::tuples::ignore) = _request_parser.parse(_request, _buffer.data(), _buffer.data() + bytes_transferred);

            if (result) {
                LOG(DEBUG) << "RESULT: " << std::string(_buffer.data(), _buffer.data() + bytes_transferred);
                _request_handler.handleRequest(_request, _reply);
                boost::asio::async_write(
                    _socket,
                    _reply.toBuffers(),
                    _strand.wrap(boost::bind(&Connection::handleWrite, shared_from_this(), boost::asio::placeholders::error)));
            }
            else if (not result) {
                LOG(DEBUG) << "NOT RESULT: " << std::string(_buffer.data(), _buffer.data() + bytes_transferred);
                _reply = Reply::stockReply(Reply::bad_request);
                boost::asio::async_write(
                    _socket,
                    _reply.toBuffers(),
                    _strand.wrap(boost::bind(&Connection::handleWrite, shared_from_this(), boost::asio::placeholders::error)));
            }
            else {
                LOG(DEBUG) << "READ: " << std::string(_buffer.data(), _buffer.data() + bytes_transferred);
                _socket.async_read_some(
                    boost::asio::buffer(_buffer),
                    _strand.wrap(boost::bind(
                        &Connection::handleRead,
                        shared_from_this(),
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred)));
            }
        }
    }


    void Connection::handleWrite(const boost::system::error_code& e) {
        if (not e) {
            boost::system::error_code ignored_ec;
            _socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }
    }
} // namespace http_server
