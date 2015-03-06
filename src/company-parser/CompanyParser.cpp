/*!
 * \brief

 * \author Rostislav Velichko.
 * \date   7.06.2012
 */

#include <limits.h>
#include <stdio.h>
#include <limits.h>
#include <signal.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <deque>
#include <queue>
#include <memory>
#include <set>
#include <cstdint>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/detail/xml_parser_flags.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/fusion/include/boost_tuple.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>

#include "RecursiveParser.hpp"
#include "RubricParser.hpp"
#include "CompanyParser.hpp"


namespace bfs = boost::filesystem;


namespace NParser {

    struct Convert {
        bool _with_url;
        std::string _rubric;
        std::string _region;
        std::stringstream _company;

        std::string changeSymbols(std::string const& str) {
            std::string res = str;

            if (not res.empty()) {
                size_t found = str.find('&');

                if (found != std::string::npos) {
                    res.erase(found);
                    res.insert(found, " and ");
                }

                found = str.find("<<");

                if (found != std::string::npos) {
                    res.erase(found);
                    res.insert(found, "\"");
                }

                found = str.find(">>");

                if (found != std::string::npos) {
                    res.erase(found);
                    res.insert(found, "\"");
                }

                found = str.find('<');

                if (found != std::string::npos) {
                    res.erase(found);
                    res.insert(found, "\"");
                }

                found = str.find('>');

                if (found != std::string::npos) {
                    res.erase(found);
                    res.insert(found, "\"");
                }
            }
            return res;
        }


        Convert(TPtreeVtype& v, std::shared_ptr< NParser::RubricParser >& rubrics)
            : _with_url(false)
            , _rubric("none")
            , _region("none")
        {
            std::string name = v.first;

            if (v.first == "Company") {
                std::string rubric_id = v.second.get< std::string >(
                    "Rubrics.rubric.<xmlattr>.ref", "rubric id"
                    );
                std::string url = v.second.get< std::string >(
                    "url", ""
                    );
                std::string location = v.second.get< std::string >(
                    "Geo.Location.gml:pos", "pos pos"
                    );

                std::string title = changeSymbols(v.second.get< std::string >(
                    "name", "name"
                    ));
                std::string address = changeSymbols(v.second.get< std::string >(
                    "Geo.AddressDetails.Country.AddressLine", ""
                    ));

                size_t location_split_pos = location.find(" ");
                std::string location_lo(location.begin(), (location.begin() + location_split_pos));
                std::string location_la((location.begin() + location_split_pos + 1), location.end());

                _rubric = changeSymbols(rubrics.get() ? rubrics->getRubric(rubric_id) : "rubric");
                _region = v.second.get< std::string >("Geo.AddressDetails.Country.AddressLine", "");

                _company << "<profile type=\"contenttype\">\n";
                _company << "    <title type=\"" << "field" << "\">" << title << "</title>\n";

                _company << "    <field_location type=\"field\">\n";
                _company << "        <latitude>" << location_la << "</latitude>\n";
                _company << "        <longitude>" << location_lo << "</longitude>\n";
                _company << "    </field_location>\n";

                _company << "    <taxonomy type=\"taxonomy\">\n";
                _company << "        <term take_parents=\"0\" vid=\"6\">" << _rubric << "</term>\n";
                _company << "    </taxonomy>\n";

                if ( ! url.empty()) {
                    _with_url = true;
                    _company << "    <field_site type=\"" << "field" << "\">\n";
                    _company << "        <value>" << url << "</value>\n";
                    _company << "    </field_site>\n";
                }

                _company << "    <field_goods type=\"" << "field" << "\">\n";
                _company << "        <value>" << "</value>\n";
                _company << "    </field_goods>\n";

                _company << "    <field_address type=\"" << "field" << "\">\n";

                if ( ! address.empty()) {
                    _company << "        <value>" << address <<"</value>\n";
                }
                else {
                    _company << "        <value></value>\n";
                }
                _company << "    </field_address>\n";

                _company << "    <field_hours type=\"" << "field" << "\">\n";
                _company << "         <value>" << "</value>\n";
                _company << "    </field_hours>\n";

                BOOST_FOREACH(TPtreeVtype& phone, v.second.get_child("Phones")) {
                    if (phone.first == "Phone") {
                        std::string phone_str = phone.second.get< std::string >("formatted");
                        _company << "    <field_phone type=\"" << "field" << "\">\n";
                        _company << "        <value>" << phone_str << "</value>\n";
                        _company << "    </field_phone>\n";
                    }
                }

                _company << "    <field_email type=\"" << "field" << "\">\n";
                _company << "        <value>""</value>\n";
                _company << "    </field_email>\n";
                _company << "</profile>\n";
            }
            else {
                RecursiveParser(v.second);
            }
        }
    };


    class OnOffFile {
        bfs::path _path;
        std::ofstream _ofs;

    public:
        std::uint32_t _count;

        OnOffFile(bfs::path name)
            : _path(name)
            , _count(1)
        {
            _ofs.open(_path.string().c_str());

            if (_ofs.is_open()) {
                //std::clog << "New file: `" << _path << "`\n";
                _ofs << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
                _ofs << "<items>\n";
                _ofs.close();
            }
            else {
                std::cerr << "Can`t open New file:`" << _path << "`\n";
            }
        }

        ~OnOffFile() {
            _ofs.open(_path.string().c_str(), std::ios::app);

            if (_ofs.is_open()) {
                _ofs << "</items>";
                _ofs.close();

                try {
                    bfs::path result_path((boost::format("%s/%s-%d%s")
                        % _path.parent_path().string()
                        % _path.stem().string()
                        % _count
                        % _path.extension().string()
                        ).str());
                    //std::clog << "Rename to:`" << result_path << "`\n";
                    bfs::rename(_path, result_path);
                }
                catch (const bfs::filesystem_error &e) {
                    std::cerr << e.what() << "\n" << std::flush;
                }
                //std::clog << "Close file:`" << _path << "`\n";
            }
            else {
                std::cerr << "Can`t close Result file:`" << _path << "`\n";
            }
        }
    };


    struct OutFile {
        bfs::path _path;
        std::ofstream _ofs;

        OutFile(bfs::path name)
            : _path(name)
        {
            _ofs.open(_path.string().c_str(), std::ios::app);

            if (not _ofs.is_open()) {
                std::cerr << "Can`t open additin file:`" << _path << "`\n";
            }
        }

        ~OutFile() {
            _ofs.close();
        }

        bool operator<< (const std::string& str) {
            if (_ofs.is_open()) {
                _ofs << str.c_str();
                return true;
            }
            return false;
        }
    };

    struct FileName {
        typedef std::shared_ptr< OnOffFile >          OnOffFilePtr;
        typedef std::shared_ptr< OutFile >            OutFilePtr;
        typedef std::map< std::string, OnOffFilePtr > Files;
        typedef Files::iterator                       FilesIter;

        static Files _files_;

        OutFilePtr _out_file;

        FileName(
            bfs::path& result,
            bool is_url,
            const std::string& region,
            const std::string& rubric
            )
        {
            bfs::path result_url_path;

            if (is_url) {
                result_url_path = bfs::path(result.string() + "/url");
            }
            else {
                result_url_path = bfs::path(result.string() + "/no-url");
            }

            if (not bfs::exists(result_url_path)) {
                bfs::create_directory(result_url_path);
            }

            bfs::path region_path(
                result_url_path.string() + "/" + correctRegionName(region));

            if (not bfs::exists(region_path)) {
                bfs::create_directory(region_path);
            }

            std::string name = region_path.string() + "/" + correctName(rubric) + ".xml";

            FilesIter it = _files_.find(name);

            if (it == _files_.end()) {
                _files_[name] = OnOffFilePtr(new OnOffFile(name));
            }
            else {
                it->second->_count++;
            }
            _out_file = OutFilePtr(new OutFile(name));
        }

        std::string correctName(const std::string& name) {
            std::string res = name;
            size_t found = res.find('/');

            while (found not_eq std::string::npos) {
                res[found] = '-';
                found = res.find('/');
            }

            return res;
        }

        std::string correctRegionName(const std::string& name) {
            if (regex_match(name, boost::regex("[А-Яа-яЁё].*"))) {
                return correctName(std::string(name, 0, name.find(',')));
            }
            else {
                size_t pos = name.rfind(',');

                if (pos not_eq std::string::npos) {
                    pos++;

                    if (name[pos] == ' ') {
                        pos++;
                    }

                    if (name.size() <= pos) {
                        pos = 0;
                    }
                }
                else {
                    pos = 0;
                }
                return correctName(name.substr(pos));
            }
        }

        bool operator<< (const std::string& str) {
            return (*_out_file) << str;
        }
    };


    FileName::Files FileName::_files_;


    using namespace NParser;

    struct SignalInit {
        SignalInit() {
            signal(SIGTERM, handle);
            signal(SIGINT, handle);
            signal(SIGQUIT, handle);
        }

        static void handle(int signal)
        {
            switch (signal)
            {
            case SIGTERM:
            case SIGINT:
            case SIGQUIT:
                CloseParsig();
                throw Stop();
                break;
            }
        }
    };


    CompanyParser::CompanyParser(
        const std::string& file_path,
        PRubricParser& rubrics,
        const std::string& complete_dir)
    {
        SignalInit();

        try {
            TPtree xml;
            int properties =
                boost::property_tree::xml_parser::trim_whitespace |
                boost::property_tree::xml_parser::no_comments;
            read_xml(file_path, xml, properties);

            bfs::path path(file_path);
            std::string res_name = bfs::basename(path);
            bfs::path result(complete_dir);

            if (not bfs::exists(result)) {
                bfs::create_directory(result);
            }
            //std::clog << "Parse in `" << complete_dir << "`; file: `" << file_path << "`\n";

            BOOST_FOREACH(TPtreeVtype& v, xml.get_child("Companies")) {
                try {
                    Convert convert(v, rubrics);
                    std::string str = convert._company.str();

#                   ifdef ALL_PARSE
                    if (not str.empty()) {
#                   else // ALL_PARSE
                    if (not str.empty() and not convert._with_url) {
#                   endif // ALL_PARSE
                        FileName file_name(
                            result,
                            convert._with_url,
                            convert._region,
                            convert._rubric
                            );

                        file_name << str;
                    }
                }
                catch (std::exception &e) {
                    std::cerr << "Error in convertion: " << e.what() << "\n" << std::flush;
                }
            }
            //std::clog << "# `" << res_name << "` is complete.\n" << std::flush;
        }
        catch (std::exception &e) {
            std::cerr << "Error in parse XML:" << e.what() << ": `" << file_path << "`\n" << std::flush;
        }
    }


    CloseParsig::CloseParsig() {
        FileName::_files_.clear();
    }
}
