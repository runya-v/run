/*!
 * \brief  Интерефейс разбивающий XML файл компаний на части.

 * \author Rostislav Velichko.
 * \date   15.06.2011
 */


#pragma once


#include <inttypes.h>

#include <string>
#include <fstream>


namespace NParser {


    class CompanyFileSeparator {
        std::ofstream _ofs;

        inline bool openResultFile(std::string const& path, uint32_t file_count);
        inline void closeResultFile();

    public:
        CompanyFileSeparator(std::string const& file, std::string const& path, uint32_t count);
    };
}
