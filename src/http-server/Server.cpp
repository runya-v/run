#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "Server.hpp"

namespace http_server {

    namespace asio     = boost::asio;

    typedef asio::ip::tcp Tcp;
    typedef boost::thread Thread;
    typedef boost::shared_ptr< Thread > PThread;
    typedef std::vector< PThread > PThreads;


    void Server::startAccept() {
        new_connection_.reset(new Connection(io_service_, request_handler_));
        acceptor_.async_accept(new_connection_->socket(), boost::bind(&Server::handleAccept, this, asio::placeholders::error));
    }


    void Server::handleAccept(const boost::system::error_code& e) {
        if (not e) {
            new_connection_->start();
        }
        startAccept();
    }


    void Server::handleStop() {
        io_service_.stop();
    }


    Server::Server(const std::string& address, const std::string& port, const std::string& doc_root, std::size_t thread_pool_size)
        : thread_pool_size_(thread_pool_size)
        , signals_(io_service_)
        , acceptor_(io_service_)
        , new_connection_()
        , request_handler_(doc_root)
    {
        signals_.add(SIGINT);
        signals_.add(SIGTERM);

#       if defined(SIGQUIT)
        signals_.add(SIGQUIT);
#       endif // defined(SIGQUIT)

        signals_.async_wait(boost::bind(&Server::handleStop, this));

        // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).

        Tcp::resolver resolver(io_service_);
        Tcp::resolver::query query(address, port);
        Tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(Tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();

        startAccept();
    }


    void Server::run() {
        // Запуск всех потоков пула.
        PThreads threads;

        for (std::size_t i = 0; i < thread_pool_size_; ++i) {
            PThread thread(new Thread(boost::bind(&asio::io_service::run, &io_service_)));
            threads.push_back(thread);
        }

        // Ожидание завершения работы всех потоков в пуле.
        for (std::size_t i = 0; i < threads.size(); ++i) {
            threads[i]->join();
        }
    }
}
