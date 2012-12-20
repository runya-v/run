/*!
 * \brief  Интерефейс, предоставляющий разбора XML файла.

 * \author Rostislav Velichko.
 * \date   15.06.2011
 */


#pragma once


#include <string>
#include <map>


namespace NParser {
    class RubricParser {
        std::map< std::string, std::string > _rubrics;

    public:
        RubricParser(std::string const& file_path);
        std::string const& getRubric(std::string const& id);
    };
}
