#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "base.hpp"
#include "Server.hpp"

namespace http_server {

namespace asio = boost::asio;

typedef asio::ip::tcp             Tcp;
typedef boost::thread             Thread;
typedef boost::shared_ptr<Thread> PThread;
typedef std::vector<PThread>      PThreads;


void Server::startAccept() {
    _new_connection.reset(new Connection(_io_service, _request_handler));
    _acceptor.async_accept(_new_connection->socket(), boost::bind(&Server::handleAccept, this, asio::placeholders::error));
}


void Server::handleAccept(const boost::system::error_code& e) {
    if (not e) {
        _new_connection->start();
    }
    startAccept();
}


void Server::handleStop() {
    _io_service.stop();
}


Server::Server(const std::string& address, const std::string& port, const std::string& doc_root, std::size_t thread_pool_size)
    : _thread_pool_size(thread_pool_size)
    , _signals(_io_service)
    , _acceptor(_io_service)
    , _new_connection()
    , _request_handler(doc_root)
{
    _signals.add(SIGINT);
    _signals.add(SIGTERM);

#   if defined(SIGQUIT)
    _signals.add(SIGQUIT);
#   endif // defined(SIGQUIT)

    _signals.async_wait(boost::bind(&Server::handleStop, this));

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).

    Tcp::resolver resolver(_io_service);
    Tcp::resolver::query query(address, port);
    Tcp::endpoint endpoint = *resolver.resolve(query);
    _acceptor.open(endpoint.protocol());
    _acceptor.set_option(Tcp::acceptor::reuse_address(true));
    _acceptor.bind(endpoint);
    _acceptor.listen();

    startAccept();
}


void Server::run() {
    // Запуск всех потоков пула.
    PThreads threads;

    for (std::size_t i = 0; i < _thread_pool_size; ++i) {
        PThread thread(new Thread(boost::bind(&asio::io_service::run, &_io_service)));
        threads.push_back(thread);
    }

    // Ожидание завершения работы всех потоков в пуле.
    for (auto thread : threads) {
        thread->join();
    }
}
}
