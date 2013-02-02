#include <fstream>
#include <sstream>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/detail/xml_parser_flags.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <Wt/WText>
#include <Wt/WWebWidget>

#include "RubrickWidget.hpp"


namespace Web {

    typedef boost::property_tree::ptree TPtree;
    typedef boost::property_tree::ptree::value_type TPtreeVtype;
    typedef boost::property_tree::ptree::const_iterator TPtreeCiter;

    namespace bfs = boost::filesystem;


    class Convert {
        std::stringstream _content;

    public:
        Convert(const TPtreeVtype &v) {
            if (v.first == "profile") {
                _content << "<h3>" << v.second.get< std::string >("title", "") << "</h3>";

                _content << "<p>ГЕО:</p>";
                _content << "<p>latitude: " << v.second.get< std::string >("field_location.latitude", "") << "</p>";
                _content << "<p>longitude: " << v.second.get< std::string >("field_location.longitude", "") << "</p>";

                std::string phones;

                try {
                    for (const TPtreeVtype &phone : v.second.get_child("field_phone")) {
                        if (phone.first == "value") {
                            phones += std::string("<p><b>") + phone.second.data() + "</b></p>";
                        }
                    }
                }
                catch (...) {}

                if (not phones.empty()) {
                        _content << "<p>Телефоны: </p>" << phones;
                }

                _content << "<p>Адреса:</p>";

                for (const TPtreeVtype &addr : v.second.get_child("field_address")) {
                    if (addr.first == "value") {
                        _content << "<p><b>" << addr.second.data() << "</b></p>";
                    }
                }

                _content << "<p>=======================================================</p>";
            }
        }

        operator std::string() {
            return _content.str();
        }
    };


    RubrickWidget::RubrickWidget(const std::string &file_name, Wt::WContainerWidget *parent)
        : Wt::WContainerWidget(parent)
        , _file_name(file_name)
    {}


    void RubrickWidget::fullContent() {
        if (_content.empty()) {
            bfs::path file_path(_file_name);

            if (bfs::exists(file_path)) {
                TPtree xml;
                int properties = boost::property_tree::xml_parser::trim_whitespace | boost::property_tree::xml_parser::no_comments;
                read_xml(bfs::path(_file_name).string(), xml, properties);
                std::string content;

                for (const TPtreeVtype &v : xml.get_child("items")) {
                    try {
                        content += Convert(v);
                    }
                    catch (std::exception &e) {
                        std::cerr << "Error in convertion: " << e.what() << "\n" << std::flush;
                    }
                }

                _content = Wt::WString::fromUTF8(content);
            }
            else {
                _content = Wt::WString::fromUTF8("Файл пустой.");
            }
        }
        this->clear();
        new Wt::WText(_content, this);
    }
}
