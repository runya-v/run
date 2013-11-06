#include <stdio.h>

#include <string>

#include <boost/utility.hpp>


namespace tmplt {
    class CFile
    	: boost::noncopyable
    {
    	FILE *_file;

    public:
    	CFile(const std::string &file_name);
        ~CFile();

        operator FILE* ();
    };
} // tmplt
