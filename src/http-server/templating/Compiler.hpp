/*!
 * \brief  Кокпилятор шаблона.
 * \author Rostislav Velichko.
 * \date   05.11.2013
 */

#include <string>
#include <vector>

#include <boost/utility.hpp>


namespace tmplt {
    typedef std::vector<char> DataBuf;


    class Compiler 
        : public boost::noncopyable
    {
        DataBuf _result;
        
    public:
        Compiler(const std::string &file_source);
        
        operator const DataBuf&();
    };
} // tmplt
