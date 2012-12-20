/*!
 * \brief

 * \author Rostislav Velichko.
 * \date   7.06.2012
 */


#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>


#include "RecursiveParser.hpp"


using namespace NParser;


RecursiveParser::RecursiveParser(TPtree xml) {
    BOOST_FOREACH(TPtreeVtype& v, xml) {
        std::string name = v.first.data();

        if (name == "<xmlattr>") {
            std::cout << "attributes: {";
            RecursiveParser(v.second);
            std::cout << "}";
        }
        else {
            std::string val = xml.get<std::string>(name);

            if (val.empty()) {
                std::cout << name << " {" <<  "\n";
                RecursiveParser(v.second);
                std::cout << "}\n";
            }
            else {
                std::cout << name << ":" << val <<  "\n";
                RecursiveParser(v.second);
                std::cout << "\n";
            }
        }
    }
}
