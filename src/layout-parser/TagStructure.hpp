/*!
 * \brief  Клас строящий структуру Теэг по html файлу.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   24.11.2012
 */


#pragma once

#include "Tag.hpp"
#include "Tree.hpp"


namespace HtmlParser {

    class TagStructure {
        TagObjs _sequence;

    public:
        TagStructure(BaseTag *parent_tag, const LibTree &tree, const LibTreeIter &parent_it);

        operator TagObjs ();
    };
} // namespace HtmlParser
