/*!
 * \brief  Генератор DOM дерева HTML файла.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   23.11.2012
 */


#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "Tree.hpp"


namespace HtmlParser {

    std::string Tree::fileText(const std::string &file_path) {
        namespace bfs = boost::filesystem;

        bfs::path in_html(file_path);

        if (bfs::is_directory(in_html)) {
            in_html /= "index.html";
        }

        if (bfs::exists(in_html)) {
            bfs::ifstream ifs(in_html.string());
            std::string line;
            std::string str;

            while (ifs) {
                std::string line;
                std::getline(ifs, line);

                if (ifs) {
                    str += line;
                }
            }
            return str;
        }
        else {
            std::cerr << "Can`t find file `" << in_html << "`\n";
        }
        return "";
    }

    Tree::Tree(const std::string &file_name) {
        html::ParserDom lib_parser;
        lib_parser.parse(fileText(file_name));
        _dom_tree = lib_parser.getTree();
    }

    Tree::operator LibTree () {
        return _dom_tree;
    }
} // namespace HtmlParser
