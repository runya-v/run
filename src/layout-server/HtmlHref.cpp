/*!
 * \brief  Класс - генератор web страницы.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   26.12.2012
 */


#include <cppcms/http_response.h>

#include "HtmlHref.hpp"


namespace LayoutParser {

    HtmlHref::HtmlHref(cppcms::service &srvc, const HtmlValue& html_val)
        : cppcms::application(srvc)
        , _html_val(html_val)
    {}


    void HtmlHref::init () {
        response().out() << _html_val.second;
    }


    std::string HtmlHref::name() {
        return _html_val.first;
    }


    std::string HtmlHref::url() {
        return _html_val.first;
    }


    std::string HtmlHref::regex() {
        return _html_val.first;
    }
} // LayoutParser
