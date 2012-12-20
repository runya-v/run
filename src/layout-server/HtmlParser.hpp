/*!
 * \brief  Класс выборки базовых данных из html странички.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   16.12.2012
 */


#include <map>
#include <set>
#include <string>

#include "HtmlTree.hpp"


namespace LayoutParser {

    typedef std::map< std::string, std::string > Htmls;
    typedef Htmls::iterator                      HtmlIter;
    typedef Htmls::value_type                    HtmlValue;

    typedef std::set< std::string > WebFiles;
    typedef WebFiles::iterator      WebFileIter;


    class HtmlParser {
        typedef std::map< std::string, std::string > LibArrts;
        typedef std::pair< bool, std::string >       LibFindAttr;

        void useAttribute(const LibFindAttr& attr, void(HtmlParser::*funk)(const std::string&));
        void insert(WebFiles& files, const std::string& value);

        std::string filePath(const std::string& href);
        void href(const std::string& value);
        void src(const std::string& value);

    public:
        static Htmls _htmls_;
        static WebFiles _styles_;
        static WebFiles _images_;

        HtmlParser(const std::string& file_path);
        HtmlParser(const LibTree &tree, const LibTreeIter &parent_it);
    };
} // LayoutParser
