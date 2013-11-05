/*!
 * \brief  Генератор данных на основе скомпилированного шаблона и ресурсных данных.
 * \author Rostislav Velichko.
 * \date   05.11.2013
 */

#include <string>

#include <boost/utility.hpp>

#include "Compiler.hpp"

namespace tmplt {
   
    class Generator
        : public boost::noncopyable
    {
    public:
        Generator(const Compiler &cmplr, const std::string &file_json, const std::string &file_html);
    };
} // tmplt
