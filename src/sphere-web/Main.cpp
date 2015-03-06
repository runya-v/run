/*!
 * \brief  Web приложение предоставляющее реализующее сайт портфолио на базе boost::asio и ct++.
 * \author Rostislav Velichko.
 * \date   01.30.2013
 */


#include <iostream>

#include <boost/filesystem.hpp>


// Пример запуска: ./companyes-web --docroot . --http-address 0.0.0.0 --http-port=8888
int main(int argc, char **argv) {
    return Wt::WRun(argc, argv, Web::CreateApp);
}
