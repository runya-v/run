#include "RequestParser.hpp"

#include "RequestParser.hpp"
#include "Request.hpp"


namespace http_server {

    RequestParser::RequestParser()
        : _state(method_start)
    {}

    void RequestParser::reset() {
        _state = method_start;
    }


    boost::tribool RequestParser::consume(Request& req, char input) {
        switch (_state) {
        case method_start:
            if (not isChar(input) or isCtl(input) or isTspecial(input)) {
                return false;
            }
            else {
                _state = method;
                req._method.push_back(input);
                return boost::indeterminate;
            }
        case method:
            if (input == ' ') {
                _state = uri;
                return boost::indeterminate;
            }
            else if (not isChar(input) or isCtl(input) or isTspecial(input)) {
                return false;
            }
            else {
                req._method.push_back(input);
                return boost::indeterminate;
            }
        case uri:
            if (input == ' ') {
                _state = http_version_h;
                return boost::indeterminate;
            }
            else if (isCtl(input)) {
                return false;
            }
            else {
                req._uri.push_back(input);
                return boost::indeterminate;
            }
        case http_version_h:
            if (input == 'H') {
                _state = http_version_t_1;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_t_1:
            if (input == 'T') {
                _state = http_version_t_2;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_t_2:
            if (input == 'T') {
                _state = http_version_p;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_p:
            if (input == 'P') {
                _state = http_version_slash;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_slash:
            if (input == '/') {
                req._http_version_major = 0;
                req._http_version_minor = 0;
                _state = http_version_major_start;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_major_start:
            if (isDigit(input)) {
                req._http_version_major = req._http_version_major * 10 + input - '0';
                _state = http_version_major;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_major:
            if (input == '.') {
                _state = http_version_minor_start;
                return boost::indeterminate;
            }
            else if (isDigit(input)) {
                req._http_version_major = req._http_version_major * 10 + input - '0';
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_minor_start:
            if (isDigit(input)) {
                req._http_version_minor = req._http_version_minor * 10 + input - '0';
                _state = http_version_minor;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case http_version_minor:
            if (input == '\r') {
                _state = expecting_newline_1;
                return boost::indeterminate;
            }
            else if (isDigit(input)) {
                req._http_version_minor = req._http_version_minor * 10 + input - '0';
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case expecting_newline_1:
            if (input == '\n') {
                _state = header_line_start;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case header_line_start:
            if (input == '\r') {
                _state = expecting_newline_3;
                return boost::indeterminate;
            }
            else if (not req._headers.empty() && (input == ' ' or input == '\t')) {
                _state = header_lws;
                return boost::indeterminate;
            }
            else if (not isChar(input) or isCtl(input) or isTspecial(input)) {
                return false;
            }
            else {
                req._headers.push_back(Header());
                req._headers.back()._name.push_back(input);
                _state = header_name;
                return boost::indeterminate;
            }
        case header_lws:
            if (input == '\r') {
                _state = expecting_newline_2;
                return boost::indeterminate;
            }
            else if (input == ' ' or input == '\t') {
                return boost::indeterminate;
            }
            else if (isCtl(input)) {
                return false;
            }
            else {
                _state = header_value;
                req._headers.back()._value.push_back(input);
                return boost::indeterminate;
            }
        case header_name:
            if (input == ':') {
                _state = space_before_header_value;
                return boost::indeterminate;
            }
            else if (not isChar(input) or isCtl(input) or isTspecial(input)) {
                return false;
            }
            else {
                req._headers.back()._name.push_back(input);
                return boost::indeterminate;
            }
        case space_before_header_value:
            if (input == ' ') {
                _state = header_value;
                return boost::indeterminate;
            }
            else {
                return false;
            }
        case header_value:
            if (input == '\r') {
                _state = expecting_newline_2;
                return boost::indeterminate;
            }
            else if (isCtl(input)) {
                return false;
            }
            else {
                req._headers.back()._value.push_back(input);
                return boost::indeterminate;
            }
        case expecting_newline_2:
            if (input == '\n') {
                _state = header_line_start;
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
