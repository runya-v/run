#pragma once

#include <boost/utility.hpp>

#include "base.hpp"
#include "utility.hpp"
#include "Generator.hpp"


namespace http_server {
    class HtmlMaker 
        : public boost::noncopyable
    {
        tmplt::GeneratorPtr _html_gen;
        std::string _html_file;
        
    public:
        HtmlMaker(const std::string &need_html_file, bool remove = false);
        
        operator const std::string();
    };
} // namespace http_server
