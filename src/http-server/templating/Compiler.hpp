/*!
 * \brief  Кокпилятор шаблона.
 * \author Rostislav Velichko.
 * \date   05.11.2013
 */

#include <string>
#include <vector>

#include <boost/utility.hpp>

#include "utility.hpp"


namespace tmplt {
    class Compiler 
        : public boost::noncopyable
    {
        FileSaverPtr _result;
        
    public:
        explicit Compiler(const base::bfs::path &&fswr);
        
        operator const std::string&() const;
        operator const std::string&();
    };
} // tmplt
