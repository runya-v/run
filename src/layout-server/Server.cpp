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

#include "HtmlParser.hpp"
#include "Server.hpp"


namespace LayoutParser {

    class HtmlHref
        : public cppcms::application
    {
        std::string _html;

    public:
        HtmlHref(cppcms::service &srvc, const std::string& html)
            : cppcms::application(srvc)
            , _html(html)
        {}

        void operator() () {
            response().out() << _html;
        }
    };


    typedef std::shared_ptr< HtmlHref > PHtmlHref;
    typedef std::set< PHtmlHref >       HtmlHrefs;
    typedef HtmlHrefs::iterator         HtmlHrefIter;


    HtmlHrefs html_hrefs_;


    Server::Server(cppcms::service &srvc)
        : cppcms::application(srvc)
    {
        HtmlParser("index.html");

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
                PHtmlHref html_href = std::make_shared< HtmlHref >(srvc, html.second);
                html_hrefs_.insert(html_href);

                dispatcher().assign(html.first, &HtmlHref::operator(), html_href.get());
                mapper().assign(html.first);
            }
        }
    }
} // LayoutParser
