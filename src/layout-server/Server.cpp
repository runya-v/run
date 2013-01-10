/*!
 * \brief  Класс web сервера на основе вёрстки.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   16.12.2012
 */


#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>

#include "HtmlHref.hpp"
#include "Server.hpp"


namespace LayoutParser {

    Server::Server(cppcms::service &srvc)
        : cppcms::application(srvc)
    {
        if (HtmlParser::_htmls_.empty()) {
            response().out() <<
                "<html>\n"
                "   <head>\n"
                "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
                "      <title>Парсер вёрстки</title>\n"
                "   </head>\n"
                "   <body>\n"
                "       <h1>Web Сервер предосталяющий сервис на базе готовой вёрстки.</h1>\n"
                "   </body>\n"
                "</html>\n";
        }
        else {
            for (const HtmlValue& html : HtmlParser::_htmls_) {
                HtmlHref* html_href = new HtmlHref(srvc, html);
                dispatcher().assign("", &HtmlHref::init, html_href);
                mapper().assign("");
                attach(
                    html_href,
                    html_href->name(),
                    html_href->url(),
                    html_href->regex(),
                    1);
            }
            dispatcher().assign("", &Server::init, this);
            mapper().assign("");
            mapper().root("");
        }
    }
} // LayoutParser
