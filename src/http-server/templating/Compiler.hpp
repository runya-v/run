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
    typedef std::vector<char> DataBuf;


    class Compiler 
        : public boost::noncopyable
    {
        std::string _file_source;
        DataBuf _result;
        
    public:
        Compiler(const FileSaver &&fswr);
        
        operator const DataBuf&();
        operator const std::string&();
    };
} // tmplt
