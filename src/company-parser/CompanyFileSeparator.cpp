/*!
 * \brief  Интерефейс разбивающий XML файл компаний на части.

 * \author Rostislav Velichko.
 * \date   7.06.2012
 */


#include <inttypes.h>

#include <iostream>
#include <sstream>
#include <cstdint>

#include <boost/filesystem.hpp>

#include "Error.hpp"
#include "CompanyFileSeparator.hpp"


namespace NParser {

    namespace bf = boost::filesystem;

    static const std::string g_control_tag("Company");
    static const uint32_t g_min_count = 2;
    static const uint32_t g_max_count = 100000;
    static const uint32_t g_buf_size  = 10000;


    inline bool CompanyFileSeparator::openResultFile(std::string const& path, uint32_t file_count) {
        std::stringstream file_name;
        file_name << path << "/" << file_count << ".xml";
        _ofs.open(file_name.str().c_str());

        if (_ofs.is_open()) {
            _ofs << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
            _ofs << "<Companies>\n";
                /*"<Companies " \
                    "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " \
                    "xmlns=\"http://maps.yandex.ru/backa/1.x\" " \
                    "xmlns:atom=\"http://www.w3.org/2005/Atom\" " \
                    "xmlns:xal=\"urn:oasis:names:tc:ciq:xsdschema:xAL:2.0\" " \
                    "xmlns:gml=\"http://www.opengis.net/gml\" " \
                    "xsi:schemaLocation=\"http://maps.yandex.ru/backa/1.x ../backa.xsd " \
                    "http://www.opengis.net/gml ../../../ymaps/ymaps/1.x/gml.xsd " \
                    "urn:oasis:names:tc:ciq:xsdschema:xAL:2.0 " \
                    "http://docs.oasis-open.org/election/external/xAL.xsd\"" \
                    ">\n";*/
            std::clog << "New file`"<< file_name.str().c_str() << "`\n" << std::flush;
        }
        return _ofs.is_open();
    }


    inline void CompanyFileSeparator::closeResultFile() {
        if (_ofs) {
            _ofs << "</Companies>";
            _ofs.close();
        }
    }


    CompanyFileSeparator::CompanyFileSeparator(
        std::string const& file, std::string const& path, uint32_t need_count
        )
    {

        if (need_count < g_min_count) {
            need_count = g_min_count;
        }
        else if (g_max_count < need_count) {
            need_count = g_max_count;
        }

        uint64_t file_size = bf::file_size(file);
        std::clog << "Size=" << file_size << '\n';

        bf::path in_file(file);

        if (bf::exists(in_file)) {
            std::string tag;
            std::string company;
            uint32_t count = need_count;
            uint32_t file_count = 0;

            std::ifstream ifs;
            ifs.open(file.c_str());

            if (ifs.is_open()) {
                std::vector< std::uint8_t > buf;
                std::string line;
                size_t pos = std::string::npos;

                while ((pos == std::string::npos) and (ifs.good())) {
                    getline(ifs, line);
                    pos = line.find("<Company ");
                }

                while (ifs.good()) {
                    if (pos not_eq std::string::npos) {
                        ++count;

                        if (need_count < count) {
                            ++file_count;
                            count = 0;
                            closeResultFile();
                            openResultFile(path, file_count);
                        }
                    }

                    if (not line.empty()) {
                        _ofs << line << "\n";
                    }
                    getline(ifs, line);
                    pos = line.find("<Company ");
                }

                if (_ofs.is_open()) {
                    closeResultFile();
                }
            }
        }
        else {
            BOOST_THROW_EXCEPTION(Error() << EStr(std::string("Can`t find file: ") + file));
        }
    }
}
