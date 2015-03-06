/*!
 * \brief  Интерефейс, предоставляющий разбора XML файла.

 * \author Rostislav Velichko.
 * \date   15.06.2011
 */


#pragma once


#include <boost/exception/all.hpp>


typedef boost::error_info< struct TagMyInfo, std::string > EStr;


struct Error
    : virtual std::exception
    , virtual boost::exception {
};
