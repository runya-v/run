#include "RequestParser.hpp"

#include "RequestParser.hpp"
#include "Request.hpp"


namespace http_server {

    RequestParser::RequestParser()
        : state_(method_start)
    {}

    void RequestParser::reset() {
        state_ = method_start;
    }


    std::string method                    = "\\w+ (/\\w+)+ HTTP/\\d.\\d\r\n(\\w)[ \\t]";
    std::string uri                       = "";
    std::string http_version_h            = "";
    std::string http_version_t_1          = "";
    std::string http_version_t_2          = "";
    std::string http_version_p            = "";
    std::string http_version_slash        = "";
    std::string http_version_major_start  = "";
    std::string http_version_major        = "";
    std::string http_version_minor_start  = "";
    std::string http_version_minor        = "";
    std::string expecting_newline_1       = "";
    std::string header_line_start         = "";
    std::string header_lws                = "";
    std::string header_name               = "";
    std::string space_before_header_value = "";
    std::string header_value              = "";
    std::string expecting_newline_2       = "";
    std::string expecting_newline_3       = "";


    boost::tribool RequestParser::consume(Request& req, char input) {
        list($method, $uri, $http) = preg_split('/\s+/', $request);
        
        switch (state_) {
        case method_start:
            if (not isChar(input) or isCtl(input) or isTspecial(input)) {
                return false;
            }
            else {
                state_ = method;
                req.method.push_back(input);
                return boost::indeterminate;
            }
        case method:
            if (input == ' ') {
                state_ = uri;
                return boost::indeterminate;
            }
            else if (not isChar(input) or isCtl(input) or isTspecial(input)) {
                return false;
            }
            else {
                req.method.push_back(input);
                return boost::indeterminate;
            }
        case uri:
            if (input == ' ') {
                state_ = http_version_h;
                return boost::indeterminate;
            }
            else if (isCtl(input)) {
                return false;
            }
            else {
                req.uri.push_back(input);
                return boost::indeterminate;
            }
        case http_version_h:
            if (input == 'H') {
                state_ = http_version_t_1;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_t_1:
            if (input == 'T') {
                state_ = http_version_t_2;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_t_2:
            if (input == 'T') {
                state_ = http_version_p;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_p:
            if (input == 'P') {
                state_ = http_version_slash;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_slash:
            if (input == '/') {
                req.http_version_major = 0;
                req.http_version_minor = 0;
                state_ = http_version_major_start;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_major_start:
            if (isDigit(input)) {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                state_ = http_version_major;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_major:
            if (input == '.') {
                state_ = http_version_minor_start;
                return boost::indeterminate;
            }
            else if (isDigit(input)) {
                req.http_version_major = req.http_version_major * 10 + input - '0';
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_minor_start:
            if (isDigit(input)) {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                state_ = http_version_minor;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_minor:
            if (input == '\r') {
                state_ = expecting_newline_1;
                return boost::indeterminate;
            }
            else if (isDigit(input)) {
                req.http_version_minor = req.http_version_minor * 10 + input - '0';
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case expecting_newline_1:
            if (input == '\n') {
                state_ = header_line_start;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case header_line_start:
            if (input == '\r') {
                state_ = expecting_newline_3;
                return boost::indeterminate;
            }
            else if (not req.headers.empty() && (input == ' ' or input == '\t')) {
                state_ = header_lws;
                return boost::indeterminate;
            }
            else if (not isChar(input) or isCtl(input) or isTspecial(input)) {
                return false;
            }
            else {
                req.headers.push_back(Header());
                req.headers.back().name.push_back(input);
                state_ = header_name;
                return boost::indeterminate;
            }
        case header_lws:
            if (input == '\r') {
                state_ = expecting_newline_2;
                return boost::indeterminate;
            }
            else if (input == ' ' or input == '\t') {
                return boost::indeterminate;
            }
            else if (isCtl(input)) {
                return false;
            }
            else {
                state_ = header_value;
                req.headers.back().value.push_back(input);
                return boost::indeterminate;
            }
        case header_name:
            if (input == ':') {
                state_ = space_before_header_value;
                return boost::indeterminate;
            }
            else if (not isChar(input) or isCtl(input) or isTspecial(input)) {
                return false;
            }
            else {
                req.headers.back().name.push_back(input);
                return boost::indeterminate;
            }
        case space_before_header_value:
            if (input == ' ') {
                state_ = header_value;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case header_value:
            if (input == '\r') {
                state_ = expecting_newline_2;
                return boost::indeterminate;
            }
            else if (isCtl(input)) {
                return false;
            }
            else {
                req.headers.back().value.push_back(input);
                return boost::indeterminate;
            }
        case expecting_newline_2:
            if (input == '\n') {
                state_ = header_line_start;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case expecting_newline_3: return (input == '\n');
        default: return false;
        }
    }


    bool RequestParser::isChar(int c) {
        return c >= 0 and c <= 127;
    }


    bool RequestParser::isCtl(int c) {
        return (c >= 0 and c <= 31) or (c == 127);
    }


    bool RequestParser::isTspecial(int c) {
        switch (c) {
        case '(': case ')': case '<': case '>': case '@':
        case ',': case ';': case ':': case '\\': case '"':
        case '/': case '[': case ']': case '?': case '=':
        case '{': case '}': case ' ': case '\t':
        return true;
        default: return false;
        }
    }


    bool RequestParser::isDigit(int c) {
        return c >= '0' && c <= '9';
    }
}
