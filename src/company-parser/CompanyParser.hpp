/*!
 * \brief  Интерефейс, предоставляющий разбора XML файла.

 * \author Rostislav Velichko.
 * \date   15.06.2011
 */


#pragma once


#include <string>
#include <memory>


namespace NParser {

    struct Stop {
    };

    class RubricParser;

    typedef std::shared_ptr< RubricParser > PRubricParser;


    class CompanyParser {
    public:
        CompanyParser(
            std::string const& file_path,
            PRubricParser& rubrics,
            std::string const& complete_dir
            );
    };

    struct CloseParsig {
        CloseParsig();
    };
}
