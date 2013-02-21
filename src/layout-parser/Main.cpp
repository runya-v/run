/*!
 * \brief  Пример простого web приложения.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   16.10.2012
 */

// Скрипт переконвертации вёрстки в utf-8
// if [ -d ./layout ]; then rm -r ./layout; fi; mkdir layout; for X in $(find ~/Загрузки/Верстка-11/ -name "*\.html"); do iconv -f WINDOWS-1251 -t UTF-8 $X  > layout/${X##*/}; done

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <cstdint>
#include <map>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

#include <html/CharsetConverter.h>
#include <html/Uri.h>
#include <html/ParserDom.h>
#include <html/utils.h>

#include "Plugin.hpp"


namespace b = boost;
namespace bfs = b::filesystem;
namespace html = htmlcxx::HTML;


namespace HtmlParser {

    typedef std::map< std::string, std::string > NodeAttributes;
    typedef NodeAttributes::value_type           NodeAttribute;
    typedef html::Node                           Node;
    typedef tree< html::Node >                   Tree;
    typedef tree< html::Node >::iterator         TreeIter;

    typedef std::vector< TreeIter > EqualChilds;
    typedef EqualChilds::iterator   EqualChildsIter;


    class FindTag {
        TreeIter _result;

    public:
        FindTag(const std::string& tag_name, const Tree &tr, const TreeIter &parent)
            : _result(tr.end())
        {
            std::uint32_t cnum = tr.number_of_children(parent);

            for (std::uint32_t i = 0; (i < cnum) and (_result == tr.end()); i++) {
                TreeIter it = tr.child(parent, i);

                if (not it->isComment()) {
                    if (it->tagName() not_eq tag_name) {
                        if (tr.number_of_children(it)) {
                            _result = FindTag(tag_name, tr, it);
                        }
                    }
                    else {
                        _result = it;
                    }
                }
            }
        }

        operator TreeIter () {
            return _result;
        }
    };


    class Attribute {
        std::string _result;

    public:
        Attribute(
            const std::string& attr_name,
            const Tree& tree,
            const TreeIter& tag,
            bool parse = true)
        {
            if (tag not_eq tree.end()) {
                if (parse) {
                    tag->parseAttributes();
                }

                if (not attr_name.empty()) {
                    _result = tag->attribute(attr_name).second;
                }
            }
        }

        operator std::string () {
            return _result;
        }
    };


    class Head {
        typedef std::vector< std::string >           EqualAttributes;
        typedef std::map< std::string, EqualChilds > Childs;
        typedef Childs::iterator                     ChildIter;

        Childs _childs;
        const Tree& _tree_;

        void add(const std::string& tag_name, const TreeIter &parent) {
            EqualChilds eq_childs;
            TreeIter it = FindTag(tag_name, _tree_, parent);

            while (it not_eq  _tree_.end()) {
                std::cout << "#" << tag_name << "\n";
                it = FindTag(tag_name, _tree_, it);
                Attribute("", _tree_, it);
                eq_childs.push_back(it);
            }

            _childs[tag_name] = eq_childs;
        }

    public:
        Head(const Tree& tree)
            : _tree_(tree)
        {
            add("base",     tree.begin());
            add("basefont", tree.begin());
            add("bgsound",  tree.begin());
            add("link",     tree.begin());
            add("meta",     tree.begin());
            add("script",   tree.begin());
            add("style",    tree.begin());
            add("title",    tree.begin());
        }

        EqualChilds child(const std::string& child_name) {
            ChildIter value = _childs.find(child_name);

            if (value not_eq _childs.end()) {
                return value->second;
            }
            return EqualChilds();
        }

        EqualAttributes childAtributes(const std::string& child_name, const std::string& attr_name) {
            EqualAttributes eq_attrs;

            for (TreeIter it : child(child_name)) {
                eq_attrs.push_back(Attribute(attr_name, _tree_, it, false));
            }
            return eq_attrs;
        }

        std::string combine(const EqualAttributes& eq_attrs) {
            std::string res;

            for (const std::string& str : eq_attrs) {
                res += str + ";";
            }
            return res;
        }

        operator std::string () {
            std::stringstream ss;

            ss << "base: \n"
                << "\thref\t='" << combine(childAtributes("base",   "href")) << "`\n"
                << "\ttarget\t='" << combine(childAtributes("base", "target")) << "`\n"
                ;
            ss << "basefont: \n"
                << "\tcolor\t='" << combine(childAtributes("basefont", "color")) << "`\n"
                << "\tface\t='" << combine(childAtributes("basefont",  "face")) << "`\n"
                << "\tsixe\t='" << combine(childAtributes("basefont",  "sixe")) << "`\n"
                ;
            ss << "bgsound: \n"
                << "\tbalance\t='" << combine(childAtributes("bgsound", "balance")) << "`\n" // Управляет балансом звука между правой и левой колонками.
                << "\tloop\t='" << combine(childAtributes("bgsound",    "loop")) << "`\n"    // Устанавливает, сколько раз проигрывать музыкальный файл.
                << "\tsrc\t='" << combine(childAtributes("bgsound",     "src")) << "`\n"     // Путь к музыкальному файлу.
                << "\tvolume\t='" << combine(childAtributes("bgsound",  "volume")) << "`\n"  // Задает громкость звучания музыки.
                ;
            ss << "link: \n"
                << "\tcharset\t='" << combine(childAtributes("link", "charset")) << "`\n" // Кодировка связываемого документа.
                << "\thref\t='" << combine(childAtributes("link",    "href")) << "`\n"    // Путь к связываемому файлу.
                << "\tmedia\t='" << combine(childAtributes("link",   "media")) << "`\n"   // Определяет устройство, для которого следует применять стилевое оформление.
                << "\trel\t='" << combine(childAtributes("link",     "rel")) << "`\n"     // Определяет отношения между текущим документом и файлом, на который делается ссылка.
                << "\tsizes\t='" << combine(childAtributes("link",   "sizes")) << "`\n"   // Указывает размер иконок для визуального отображения.
                << "\ttype\t='" << combine(childAtributes("link",    "type")) << "`\n"    // MIME-тип данных подключаемого файла.
                ;
            ss << "meta: \n"
                << "\tcharset\t='" << combine(childAtributes("meta",    "charset")) << "`\n"    // Задает кодировку документа.
                << "\tcontent\t='" << combine(childAtributes("meta",    "content")) << "`\n"    // Устанавливает значение атрибута, заданного с помощью name или http-equiv.
                << "\thttp-equiv\t='" << combine(childAtributes("meta", "http-equiv")) << "`\n" // Предназначен для конвертирования метатега в заголовок HTTP.
                << "\tname=`" << combine(childAtributes("meta",       "name")) << "`\n"       // Имя метатега, также косвенно устанавливает его предназначение.
                ;
            ss << "script: \n"
                << "\tdefer\t='" << combine(childAtributes("script",    "defer")) << "`\n"    // Откладывает выполнение скрипта до тех пор, пока вся страница не будет загружена полностью.
                << "\tlanguage\t='" << combine(childAtributes("script", "language")) << "`\n" // Устанавливает язык программирования на котором написан скрипт.
                << "\tsrc\t='" << combine(childAtributes("script",      "src")) << "`\n"      // Адрес скрипта из внешнего файла для импорта в текущий документ.
                << "\ttype\t='" << combine(childAtributes("script",     "type")) << "`\n"     // Определяет тип содержимого тега <script>.
                ;
            ss << "style: \n"
                << "\tmedia\t='" << combine(childAtributes("style", "media")) << "`\n" // Определяет устройство вывода, для работы с которым предназначена таблица стилей.
                << "\ttype\t='" << combine(childAtributes("style",  "type")) << "`\n"  // Сообщает браузеру, какой синтаксис использовать, чтобы правильно интерпретировать стили.
                ;
            return ss.str();
        }
    };


    class Parser {
        int _root_depth;

    public:
        Parser(const std::string &file_name) {
            std::string title;
            std::string body;

            Tree tree;
            html::ParserDom parser;
            parser.parse(getHtmlString(file_name));
            tree = parser.getTree();

            TreeIter it  = tree.begin();
            TreeIter end = tree.end();

            std::cout << (std::string)Head(tree) << "\n";

            _root_depth = tree.depth(it);

            while (it not_eq end) {
                //std::string shift = getShift(tr.depth(it));

                if (not it->tagName().empty() and not it->isComment()) {
                    if (it->isTag()) {
                        if (it->tagName() == "body") {
                            body = parse(tree, it);
                        }
                    }
                }
                ++it;
            }

            std::cout << "title: " << title << std::flush;
            //std::cout << "body: " << body << std::flush;
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

        std::string parse(const Tree &tr, const TreeIter &parent) {
            std::uint32_t cnum = tr.number_of_children(parent);
            std::string res;

            for (std::uint32_t i = 0; i < cnum; i++) {
                TreeIter it = tr.child(parent, i);

                if (not it->isComment()) {
                    std::string shift = getShift(tr.depth(it));

                    if (tr.number_of_children(it)) {
                        res += (b::format("%s%s%s%s%s%s%s")
                            % shift % it->text() % "\n"
                            % parse(tr, it)
                            % shift % it->closingText() % "\n").str();
                    }
                    else {
                        res += (b::format("%s%s%s") % shift % it->text() % "\n").str();
                    }
                }
            }
            return res;
        }
    };
}

class LayoutParser {
public:
    LayoutParser(const std::string &path) {
        namespace hp = HtmlParser;
        hp::Parser index(path);
        //std::cout << "path: `" << path << "`" << std::endl;

    }
};

#include <regex>

class LpMain
    : public Plugin
{
public:
    LpMain()
    {}

    bool execute() {
        try {
            namespace bpo = boost::program_options;
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

            std::cout
                << "Src path: `" << (vm["path"].as< std::string >() + "/" + "index.html") << "`\n";



            try {
                std::regex myregex ("*");
                std::regex gLoginRegex("^[a-zA-Z0-9_-]{3,16}$");
            } catch (std::regex_error& e) {
                if (e.code() == std::regex_constants::error_badrepeat)
                std::cerr << "Repeat was not preceded by a valid regular expression.\n";
                else std::cerr << "Some other regex exception happened.\n";
            }

            LayoutParser lo_parser(vm["path"].as< std::string >() + "/" + "index.html");
            return true;
        }
        catch(const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }
} lp_main;

