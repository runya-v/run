/*!
 * \brief  Плагиновая обёртка web сервера.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   16.12.2012
 */


#include <cppcms/applications_pool.h>
#include <cppcms/service.h>
#include "HtmlParser.hpp"
//#include "Server.hpp"
#include "Error.hpp"
#include "Plugin.hpp"


class Main
    : public Plugin
{
public:
    Main() {
    }

    bool execute() {
        namespace lo = LayoutParser;
        lo::HtmlParser("index.html");

        //for (const HtmlValue& html : HtmlParser::_htmls_) {
        //    PHtmlHref html_href = std::make_shared< HtmlHref >(srvc, html.second);
        //    html_hrefs_.insert(html_href);
        //
        //    dispatcher().assign(html.first, &HtmlHref::operator(), html_href.get());
        //    mapper().assign(html.first);
        //}
        //try {
        //    cppcms::service srv(Plugin::_argc, Plugin::_argv);
        //    srv.applications_pool().mount(cppcms::applications_factory< lo::Server >());
        //    srv.run();
        //}
        //catch(const std::exception &e) {
        //    std::cerr << e.what() << std::endl;
        //    return false;
        //}
        return true;
    }
} layout_server_;

