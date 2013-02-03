/*!
 * \brief  ASIO сервер обслуживающий TCP соединения.
 * \author Rostislav Velichko.
 * \date   01.30.2013
 */

# pragma once

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include "connection.hpp"
#include "request_handler.hpp"


namespace http_server {

    class Server
      : private boost::noncopyable
    {
        //! \brief Количество потоков в пуле потоков вызываемых io_service::run().
        std::size_t thread_pool_size_;

        //! \brief Объект выполняющий асинхронные операции.
        boost::asio::io_service io_service_;

        //! \brief Сигнал используемый в процессе уведомления о закрытии.
        boost::asio::signal_set signals_;

        //! \brief Объект обслуживающий новые подключения.
        boost::asio::ip::tcp::acceptor acceptor_;

        //! \brief Объект нового соединения для обработки.
        connection_ptr new_connection_;

        //! \brief Объект обработчик всех входящих запросов.
        request_handler request_handler_;

        /*!
         * \brief Метод инициализации асинхронной операции приёма соединений.
         */
        void startAccept();

        /*!
         * \brief Метод оповещающий о завершении асинхронной операции приёма соединений.
         * \param[in] Код ошибки.
         */
        void handleAccept(const boost::system::error_code& e);

        /*!
         * \brief Метод оповещающий о завершении запроса на остановку сервера.
         */
        void handleStop();

    public:
        /*!
         * \brief Класс верхнего уровня цикла обработки http.
         * \param[in] Адрес сервера.
         * \param[in] Порт сервера.
         * \param[in] Рабочая директория сервера.
         * \param[in] Размер пула потоков.
         */
        explicit Server(const std::string& address, const std::string& port, const std::string& doc_root, std::size_t thread_pool_size);

        /*!
         * \brief Запуск серверного цикла io_service.
         */
        void run();
    };
} // namespace http
