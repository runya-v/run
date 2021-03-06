/*!
 * \brief  Генератор DOM дерема HTML файла.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   23.11.2012
 */


#pragma once

#include <string>
#include <map>

#include "htmlcxx/html/CharsetConverter.h"
#include "htmlcxx/html/Uri.h"
#include "htmlcxx/html/ParserDom.h"
#include "htmlcxx/html/utils.h"


namespace LayoutParser {

    namespace html = htmlcxx::HTML;

    typedef html::Node                LibNode;
    typedef tree< LibNode >           LibTree;
    typedef tree< LibNode >::iterator LibTreeIter;


    class HtmlTree {
        LibTree _dom_tree;
        std::string _html_text;

    public:
        static std::string fileText(const std::string &file_name);

        HtmlTree(const std::string &file_name);

        operator std::string ();
        operator LibTree ();
    };
} // namespace HtmlParser
