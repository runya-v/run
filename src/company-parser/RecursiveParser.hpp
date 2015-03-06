/*!
 * \brief  Интерефейс, предоставляющий разбора XML файла.

 * \author Rostislav Velichko.
 * \date   15.06.2011
 */


#pragma once


#include <boost/property_tree/ptree.hpp>


namespace NParser {

    typedef boost::property_tree::ptree TPtree;
    typedef boost::property_tree::ptree::value_type TPtreeVtype;


    struct RecursiveParser {

        /*!
         * \brief
         * \param[ou]
         * \param[in]
         */
        RecursiveParser(TPtree);
    };
}
