/*!
 * \brief  Класс выборки базовых данных из html странички.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   16.12.2012
 */


#include <iostream>

#include <boost/filesystem.hpp>

#include "HtmlParser.hpp"


namespace LayoutParser {

    namespace bfs = boost::filesystem;


    Htmls HtmlParser::_htmls_;
    WebFiles HtmlParser::_styles_;
    WebFiles HtmlParser::_images_;


    void HtmlParser::useAttribute(
        const LibFindAttr& find_attr, void(HtmlParser::*use_func)(const std::string&))
    {
        if (find_attr.first) {
            (this->*use_func)(find_attr.second);
        }
    }


    void HtmlParser::insert(WebFiles& files, const std::string& file) {
        WebFileIter it = files.find(file);

        if (it == files.end()) {
            files.insert(file);
            std::clog << "New file: `" << file << "`\n";
        }
    }


    std::string HtmlParser::filePath(const std::string& href) {
        bfs::path file_path = bfs::path(href);
        bfs::path parent_path = file_path.parent_path();

        if (parent_path not_eq bfs::path(".")) {
            file_path = parent_path / file_path.filename();
        }
        else {
            file_path = file_path.filename();
        }

        if (bfs::exists(file_path)) {
            return file_path.string();
        }
        std::clog << "Can`t find file: `" << file_path.string() << "`.\n";
        return "";
    }


    void HtmlParser::href(const std::string& value) {
        if (value.find(".html") not_eq std::string::npos) {
            HtmlParser(filePath(value));
        }

        if ((value.find(".css") not_eq std::string::npos) or
            (value.find(".js") not_eq std::string::npos))
        {
            insert(_styles_, filePath(value));
        }

        if (value.find(".png") not_eq std::string::npos) {
            insert(_images_, filePath(value));
        }
    }

    void HtmlParser::src(const std::string& value) {
        if (value.find(".png") not_eq std::string::npos) {
            insert(_images_, filePath(value));
        }
    }


    HtmlParser::HtmlParser(const std::string& file_path) {
        HtmlIter html_it = _htmls_.find(file_path);

        if (html_it == _htmls_.end()) {
            std::clog << "New html: `" << file_path << "`\n";
            HtmlTree tree((bfs::current_path() / bfs::path(file_path)).string());
            _htmls_[file_path] = tree;
            HtmlParser(tree, ((LibTree)tree).begin());
        }
    }


    HtmlParser::HtmlParser(const LibTree &tree, const LibTreeIter &parent_it) {
        std::uint32_t cnum = tree.number_of_children(parent_it);

        for (std::uint32_t i = 0; i < cnum; i++) {
            LibTreeIter it = tree.child(parent_it, i);

            if (not it->isComment() and it->isTag()) {
                it->parseAttributes();

                useAttribute(it->attribute("href"), &HtmlParser::href);
                useAttribute(it->attribute("src"), &HtmlParser::src);

                if (tree.number_of_children(it)) {
                    HtmlParser(tree, it);
                }
            }
        }
    }
} // LayoutParser
