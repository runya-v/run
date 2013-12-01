#pragma once

#include <stdio.h>

#include <string>

#include <boost/utility.hpp>

#include "base.hpp" 


namespace tmplt {
    class FileSaver 
        : boost::noncopyable
    {
        base::bfs::path _path;
        
    public:
        FileSaver(const char *source, const uint32_t size, const std::string &file_name, bool binary = false);
        ~FileSaver();
        
        operator const std::string&() const;
        operator const std::string&();
    };
    
    typedef std::shared_ptr<FileSaver> FileSaverPtr;
////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    class CFile
    	: boost::noncopyable
    {
    	FILE *_file;

    public:
    	CFile(const std::string &file_name, const std::string &options = "w");
        ~CFile();

        operator FILE* ();
    };

    typedef std::shared_ptr<CFile> CFilePtr;
////////////////////////////////////////////////////////////////////////////////////////////////////

    
    class StdoutToFile 
        : boost::noncopyable
    {
        int _saved_stdout;
        FILE *_file;
        
    public:
        StdoutToFile(const std::string &file_name);
        ~StdoutToFile();
    };

    typedef std::shared_ptr<StdoutToFile> StdoutToFilePtr;
} // tmplt
