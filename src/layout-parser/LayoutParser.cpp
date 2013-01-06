/*!
 * \brief  Парсер верстки.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   17.11.2012
 */

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <cstdint>
#include <memory>
#include <map>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include "Plugin.hpp"
#include "TagCreator.hpp"
#include "Tree.hpp"
#include "Tag.hpp"
#include "TagStructure.hpp"


class LayoutParser
    : public Plugin
{
public:
    LayoutParser()
    {}

    bool execute() {
        try {
            namespace bfs = boost::filesystem;
            namespace bpo = boost::program_options;
            namespace hp = HtmlParser;

            std::string src_path = (bfs::current_path()).string();

            bpo::options_description desc("");
            desc.add_options()
                ("help,h", "help.")
                ("path,p", bpo::value< std::string >()->default_value(src_path), "Path to web leayout sources.")
                ;
            bpo::variables_map vm;
            bpo::store(bpo::parse_command_line(Plugin::_argc, Plugin::_argv, desc), vm);
            bpo::notify(vm);

            if (vm.count("help")) {
                std::cout << desc << std::endl;
                return true;
            }

            std::cout << "Src path: `" << vm["path"].as< std::string >() << "`\n";

            hp::Tree tree(vm["path"].as< std::string >());
            hp::LibTree lib_tree = tree;
            hp::TagObjs tag_objs = hp::TagStructure(NULL, lib_tree, lib_tree.begin());

            //if (not tag_objs.empty()) {
            //    hp::PTag& head_tag = tag_objs[0];
            //    std::cout << std::string(*head_tag);
            //}

            return true;
        }
        catch(const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
} lauout_parser;
