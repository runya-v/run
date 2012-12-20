/*!
 * \brief  Привет распарсивания html
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   12.10.2012
 */


#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "html/CharsetConverter.h"
#include "html/Uri.h"
#include "html/ParserDom.h"
#include "html/utils.h"

#include "Plugin.hpp"


// iconv -f WINDOWS-1251 -t UTF-8 index-1.html > index-1.html
// sed "s/\r//" in > out

//<?xml version="1.0" encoding="UTF-8"?>
//<items>
//<snip1 type="contenttype">
//    <title type="field">Заголовок</title>
//    <body type="field">
//        <value>весь текст</value>
//    </body>
//</snip1>
//</items>


namespace b = boost;
namespace bfs = b::filesystem;
namespace html = htmlcxx::HTML;


class HtmlParser {
    typedef std::map<std::string, std::string> NodeAttrs;
    typedef NodeAttrs::value_type              NodeAttr;
    typedef html::Node                         Node;
    typedef tree< html::Node >                 Tree;
    typedef tree< html::Node >::iterator       TreeIter;

    bool _result;
    int _root_depth;

public:
    HtmlParser(const std::string &file_name)
        : _result(false)
    {
        std::string title;
        std::string body;

        Tree tr;
        html::ParserDom parser;
        parser.parse(getHtmlString(file_name));
        tr = parser.getTree();

        TreeIter it  = tr.begin();
        TreeIter end = tr.end();

        _root_depth = tr.depth(it);

        while (it != end) {
            std::string shift = getShift(tr.depth(it));

            if (not it->tagName().empty() && not it->isComment()) {
                if (it->isTag()) {
                    if (it->tagName() == "head") {
                        title = parseHead(tr, it);
                    }

                    if (it->tagName() == "body") {
                        body = (b::format("<![CDATA[\n%s%s%s%s%s%s%s%s]]>")
                            % shift % it->text() % "\n" % parseBody(tr, it) % shift % it->closingText() % "\n" % shift
                            ).str();
                    }
                }
            }
            ++it;
        }

        b::format res = b::format(
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<items>\n"
            "   <snip1 type=\"contenttype\">\n"
            "       <title type=\"field\">\n"
            "           %s\n"
            "       </title>\n"
            "       <body type=\"field\">\n"
            "           <value>\n"
            "                %s\n"
            "            </value>\n"
            "       </body>\n"
            "   </snip1>\n"
            "</items>\n"
            )
            % title
            % body
            ;
        std::cout << res.str() << std::flush;

        _result = true;
    }

    std::string getHtmlString(const std::string &file_name) {
        bfs::path in_html(file_name);

        if (bfs::exists(in_html)) {
            bfs::ifstream ifs(in_html.string());
            std::string line;
            std::string html_str;

            while (ifs) {
                std::string line;
                std::getline(ifs, line);

                if (ifs) {
                    html_str += line;
                }
            }
            return html_str;
        }
        else {
            std::cerr << "Can`t find file `" << file_name << "`\n";
        }
        return "";
    }

    std::string getShift(int cur_depth) {
        std::string shift;

        for (int i = 0; i < (cur_depth - _root_depth); ++i) {
            shift += "  ";
        }
        return (shift + "                ");
    }

    std::string parseHead(const Tree &tr, const TreeIter &parent) {
        for (std::uint32_t i = 0; i < tr.number_of_children(parent); i++) {
            TreeIter it = tr.child(parent, i);

            if (not it->isComment()) {
                if (it->isTag()) {
                    if (tr.number_of_children(it)) {
                        TreeIter child = it;
                        ++child;

                        if (it->tagName() == "title") {
                            return child->text();
                        }
                    }
                }
            }
        }
        return "";
    }

    std::string parseBody(const Tree &tr, const TreeIter &parent) {
        std::uint32_t cnum = tr.number_of_children(parent);
        std::string res;

        for (std::uint32_t i = 0; i < cnum; i++) {
            TreeIter it = tr.child(parent, i);

            if (not it->isComment()) {
                std::string shift = getShift(tr.depth(it));

                if (tr.number_of_children(it)) {
                    res += (b::format("%s%s%s%s%s%s%s") % shift % it->text() % "\n" % parseBody(tr, it) % shift % it->closingText() % "\n").str();
                }
                else {
                    res += (b::format("%s%s%s") % shift % it->text() % "\n").str();
                }
            }
        }
        return res;
    }

    operator bool () {
        return _result;
    }

};


class MyPlugin
    : public Plugin
{
public:
    MyPlugin()
    {}

    bool execute() {
        if (Plugin::_argc > 1) {
            return HtmlParser(Plugin::_argv[1]);
        }
        return false;
    }

} plugin;






    //void parseMeta(tree< html::Node >::pre_order_iterator &it, tree< html::Node >::pre_order_iterator &end) {
    //    it->parseAttributes();
    //    std::vector< std::string > strs;
    //
    //    BOOST_FOREACH(const NodeAttr &attr, it->attributes()) {
    //        if (attr.first not_eq "http-equiv") {
    //            strs.push_back(attr.second);
    //        }
    //    }
    //    std::reverse(strs.begin(), strs.end());
    //
    //    BOOST_FOREACH(const std::string &str, strs) {
    //        _meta += str + " ";
    //    }
    //    _meta += "\n";
    //}


                //std::string shift = boost::lexical_cast< std::string >(cur_depth - rootdepth);

//    bool execute() {
//        if (Plugin::_argc > 1) {
//            namespace html = htmlcxx::HTML;
//            typedef std::map<std::string, std::string> NodeAttrs;
//            typedef NodeAttrs::value_type NodeAttr;
//
//            tree< html::Node > tr;
//            html::ParserDom parser;
//            parser.parse(getHtmlString(Plugin::_argv[1]));
//            tr = parser.getTree();
//
//            tree< html::Node >::pre_order_iterator it  = tr.begin();
//            tree< html::Node >::pre_order_iterator end = tr.end();
//
//            //tree< html::Node >::post_order_iterator it  = tr.begin_post();
//            //tree< html::Node >::post_order_iterator end = tr.end_post();
//
//            int rootdepth = tr.depth(it);
//
//            while (it != end) {
//                //fixed_depth_iterator begin_fixed(const iterator_base&, unsigned int) const;
//                //fixed_depth_iterator end_fixed(const iterator_base&, unsigned int) const;
//
//                html::Node n = *it;
//
//                if (not n.tagName().empty() && not n.isComment()) {
//                    int cur_depth = tr.depth(it);
//                    //std::string shift = boost::lexical_cast< std::string >(cur_depth - rootdepth);
//                    std::string shift;
//
//                    for (int i = 0; i < cur_depth - rootdepth; ++i) {
//                        shift += "  ";
//                    }
//
//                    if (n.isTag()) {
//                        std::cout << shift << "tag: `" << n.tagName() << "`" << std::endl;
//                    }
//                    else {
//                        //std::cout << shift << n.text() << std::endl;
//                        std::cout << shift << "txt: " << n.text() << std::endl;
//                    }
//                }
//                ++it;
//            }
//        }
//        return true;
//    }


            //std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            //std::cout << "<items>\n";
            //std::cout << "</items>\n";

            //std::cout << tree << "\n";

            //BOOST_FOREACH(html::Node& n, std::make_pair(tree.begin(), tree.end()))
            //{
                    //if (n.isTag() && (n.tagName().length() > 1) && not n.isComment()) {
                    //    std::cout << "tagName: " << n.tagName() << std::endl;
                    //}
                    //
                    //if (not n.isTag() && (n.tagName().length() > 1) && not n.isComment()) {
                    //    //std::cout << "@@@ {: " << std::endl;
                    //    //std::cout << "offset: " << n.offset() << std::endl;
                    //
                    //    //std::cout << "length: " << n.length() << std::endl;
                    //    std::cout << "text: " << n.text() << std::endl;
                    //    //std::cout << "tagName: " << n.tagName() << std::endl;
                    //    //std::cout << "closingText: " << n.closingText() << std::endl;
                    //
                    //    //n.parseAttributes();
                    //    //
                    //    //BOOST_FOREACH(const NodeAttr &attr, n.attributes()) {
                    //    //.std::cout << "\t$: " << attr.first << ": " << attr.second << std::endl;
                    //    //}
                    //}
                    //else if (not n.isComment()) {
                    //    std::cout << "text: " << n.text() << std::endl;
                    //    std::cout << "tagName: " << n.tagName() << std::endl;
                    //    std::cout << "@@@ }" << std::endl;
                    //}

                    //if (node.isTag()) {
                    //    std::cout << node.tagName() << ":";
                    //
                    //
                    //    //std::cout << "ct: " << node.closingText() << std::endl;
                    //}
                    //else {
                    //    std::cout << node.text() << std::endl;
                    //}
                    //
                    ////else {
                    //    //std::cout << "t#: \n" << node.text() << std::endl;
                    ////}
            //}
