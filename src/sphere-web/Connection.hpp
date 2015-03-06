/*!
 * \brief  Класс обработки клиентского подключения.
 * \author Rostislav Velichko.
 * \date   22.10.2013
 */


#include <memory>
#include <chrono>

#include "base.hpp"


namespace sphweb {
    class Connection
        : public std::enable_shared_from_this<Connection>
        , boost::noncopyable 
    {
        enum { 
            max_msg = 1024 
        };
        
        base::TcpSocket _sock;
        base::DeadLineTimer _timer;
        base::HighResolutionClock _last_ping;
        char _read_buffer[max_msg];
        char _write_buffer[max_msg];
        bool _started;
        bool _clients_changed;
        std::string _user_name;

        Connection() : sock_(service), started_(false), 
                           timer_(service), clients_changed_(false) {
        }

        void onRead(const error_code &err, size_t bytes) {
            if ( err) stop();
            if ( !started() ) return;
            // process the msg
            std::string msg(read_buffer_, bytes);
            if ( msg.find("login ") == 0) on_login(msg);
            else if ( msg.find("ping") == 0) on_ping();
            else if ( msg.find("ask_clients") == 0) on_clients();
            else std::cerr << "invalid msg " << msg << std::endl;
        }
        
        void onLogin(const std::string &msg) {
            std::istringstream in(msg);
            in >> username_ >> username_;
            std::cout << username_ << " logged in" << std::endl;
            do_write("login ok\n");
            update_clients_changed();
        }
        
        void onPing() {
            do_write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
            clients_changed_ = false;
        }
        
        void onClients() {
            std::string msg;
            for( array::const_iterator b = clients.begin(), e = clients.end() ; b != e; ++b)
                msg += (*b)->username() + " ";
            do_write("clients " + msg + "\n");
        }

        void doPing() {
            do_write("ping\n");
        }
        void doAskClients() {
            do_write("ask_clients\n");
        }

        void onCheckPing() {
            boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
            if ( (now - last_ping).total_milliseconds() > 5000) {
                std::cout << "stopping " << username_ << " - no ping in time" << std::endl;
                stop();
            }
            last_ping = boost::posix_time::microsec_clock::local_time();
        }
        void postCheckPing() {
            timer_.expires_from_now(boost::posix_time::millisec(5000));
            timer_.async_wait( MEM_FN(on_check_ping));
        }


        void onWrite(const error_code & err, size_t bytes) {
            do_read();
        }

        void doRead() {
            async_read(sock_, buffer(read_buffer_), 
                       MEM_FN2(read_complete,_1,_2), MEM_FN2(on_read,_1,_2));
            post_check_ping();
        }

        void doWrite(const std::string & msg) {
            if ( !started() ) return;
            std::copy(msg.begin(), msg.end(), write_buffer_);
            sock_.async_write_some( buffer(write_buffer_, msg.size()), 
                                    MEM_FN2(on_write,_1,_2));
        }

        size_t readComplete(const boost::system::error_code & err, size_t bytes) {
            if ( err) return 0;
            bool found = std::find(read_buffer_, read_buffer_ + bytes, '\n') < read_buffer_ + bytes;
            // we read one-by-one until we get to enter, no buffering
            return found ? 0 : 1;
        }
        
    public:
        void start() {
            started_ = true;
            clients.push_back( shared_from_this());
            last_ping = boost::posix_time::microsec_clock::local_time();
            // first, we wait for client to login
            do_read();
        }
        static ptr new_() {
            ptr new_(new talk_to_client);
            return new_;
        }
        void stop() {
            if ( !started_) return;
            started_ = false;
            sock_.close();

            ptr self = shared_from_this();
            array::iterator it = std::find(clients.begin(), clients.end(), self);
            clients.erase(it);
            update_clients_changed();
        }
        bool started() const { return started_; }
        ip::tcp::socket & sock() { return sock_;}
        std::string username() const { return username_; }
        void set_clients_changed() { clients_changed_ = true; }
    };     
}
