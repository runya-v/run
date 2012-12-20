/*!
 * \brief  Универсальный клас генерации исключения.

 * \author Rostislav Velichko.
 * \date   15.06.2011
 */


#pragma once

#include <boost/exception/all.hpp>


struct Error
    : virtual std::exception
    , virtual boost::exception
{
    typedef boost::error_info< struct ErroInfo, std::string > Msg;
};
