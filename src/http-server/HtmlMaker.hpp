#pragma once

#include <boost/utility.hpp>

#include "base.hpp"


namespace http_server {
    class HtmlMaker 
        : public boost::noncopyable
    {
        base::bfs::path _html_path;
        
    public:
        HtmlMaker(const std::string &need_html_file);
        
        operator const std::string&();
    };
} // namespace tmplt
