/*!
 * \brief  Интерефейс, предоставляющий разбора XML файла.

 * \author Rostislav Velichko.
 * \date   15.06.2011
 */


#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include "RecursiveParser.hpp"
#include "RubricParser.hpp"


namespace NParser {

    /*!
     * \brief
     * \param[ou]
     * \param[in]
     */
    struct Parse {
        Parse(TPtree xml, std::map< std::string, std::string >& rubrics) {
            BOOST_FOREACH(TPtreeVtype& v, xml) {
                std::string name = v.first;

                if (v.first == "Rubrics") {
                    BOOST_FOREACH(TPtreeVtype& v, v.second) {
                        std::string id = v.second.get< std::string >("<xmlattr>.id", "");
                        std::string name = v.second.get< std::string >("name", "");

                        if (not id.empty() && not name.empty()) {
                            rubrics[id] = name;
                        }
                    }
                }
            }
        }
    };
}


using namespace NParser;

namespace bfs = boost::filesystem;


RubricParser::RubricParser(std::string const& file_path) {
    try {
        TPtree xml;
        read_xml(file_path, xml, boost::property_tree::xml_parser::trim_whitespace);
        Parse(xml, _rubrics);
        std::cout << "All rubrics parse.\n" << std::flush;
    }
    catch (std::exception &e) {
        std::cout << e.what() << "\n" << std::flush;
    }
}


std::string const& RubricParser::getRubric(std::string const& id) {
    std::map< std::string, std::string >::iterator iter = _rubrics.find(id);

    if (iter != _rubrics.end()) {
        return iter->second;
    }
    else {
        return id;
    }
}
