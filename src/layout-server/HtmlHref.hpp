/*!
 * \brief  Класс - генератор web страницы.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   26.12.2012
 */


#include <string>

#include <cppcms/application.h>

#include "HtmlParser.hpp"


namespace LayoutParser {

    class HtmlHref
        : public cppcms::application
    {
        HtmlValue _html_val;

    public:
        HtmlHref(cppcms::service &srvc, const HtmlValue& html_val);

        void init ();

        std::string name();
        std::string url();
        std::string regex();
    };
} // LayoutParser
