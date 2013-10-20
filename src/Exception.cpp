#include "Exception.hpp"

using namespace base;


Exception::~Exception() throw()
{}


const char* Exception::what() const throw() {
    return boost::diagnostic_information_what(*this);
}


const std::string& Exception::message() const {
    if (const std::string* info = get<ErrorInfo>()) {
        _what = *info;
    }
    return _what;
}


const std::string& Exception::where() const {
    std::ostringstream where_info;

    if (const char* const *file_name = boost::get_error_info<boost::throw_file>(*this)) {
        where_info << *file_name;

        if (int const* line = boost::get_error_info<boost::throw_line>(*this)) {
            where_info << " `" << *line << "`";
        }
    }

    if (const char* const *function_name = boost::get_error_info<boost::throw_function>(*this)) {
        where_info << " in " << *function_name;
    }
    _where.assign(where_info.str());
    return _where;
}

