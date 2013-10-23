#include <vector>

#include <boost/bind.hpp>

#include "RequestHandler.hpp"
#include "Connection.hpp"
#include "Log.hpp"


namespace http_server {

    Connection::Connection(boost::asio::io_service& io_service, RequestHandler &handler)
        : strand_(io_service)
        , socket_(io_service)
        , request_handler_(handler)
    {}


    boost::asio::ip::tcp::socket& Connection::socket() {
        return socket_;
    }


    void Connection::start() {
        socket_.async_read_some(
            boost::asio::buffer(buffer_),
            strand_.wrap(boost::bind(
                &Connection::handleRead, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)));
    }


    void Connection::handleRead(const boost::system::error_code& e, std::size_t bytes_transferred) {
        if (not e) {
            boost::tribool result;
            boost::tie(result, boost::tuples::ignore) = request_parser_.parse(request_, buffer_.data(), buffer_.data() + bytes_transferred);

            if (result) {
                LOG(METHOD, base::Log::Level::DEBUG, "RESULT: " + std::string(buffer_.data(), buffer_.data() + bytes_transferred));
                request_handler_.handleRequest(request_, reply_);
                boost::asio::async_write(
                    socket_,
                    reply_.toBuffers(),
                    strand_.wrap(boost::bind(&Connection::handleWrite, shared_from_this(), boost::asio::placeholders::error)));
            }
            else if (not result) {
                LOG(METHOD, base::Log::Level::DEBUG, "NOT RESULT: " + std::string(buffer_.data(), buffer_.data() + bytes_transferred));
                reply_ = Reply::stockReply(Reply::bad_request);
                boost::asio::async_write(
                    socket_,
                    reply_.toBuffers(),
                    strand_.wrap(boost::bind(&Connection::handleWrite, shared_from_this(), boost::asio::placeholders::error)));
            }
            else {
                LOG(METHOD, base::Log::Level::DEBUG, "READ: " + std::string(buffer_.data(), buffer_.data() + bytes_transferred));
                socket_.async_read_some(
                    boost::asio::buffer(buffer_),
                    strand_.wrap(boost::bind(
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
            socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }
    }
} // namespace http_server
