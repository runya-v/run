/*!
 * \brief  Генератор данных на основе скомпилированного шаблона и ресурсных данных.
 * \author Rostislav Velichko.
 * \date   05.11.2013
 */

#include <string>
#include <memory>

#include <boost/utility.hpp>

#include "base.hpp"
#include "Compiler.hpp"

namespace tmplt {
   
    class Generator
        : public boost::noncopyable
    {
        bool _remove;
        base::bfs::path _path_result;
        
    public:
        explicit Generator(
            const base::bfs::path &file_ct2, 
            const base::bfs::path &file_json, 
            const base::bfs::path &file_html, 
            bool remove);
        ~Generator();
        
        operator const std::string&(); 
    };
    
    typedef std::shared_ptr<Generator> GeneratorPtr;
} // tmplt
