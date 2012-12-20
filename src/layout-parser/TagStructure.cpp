/*!
 * \brief  Клас строящий структуру Тэг по html файлу.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   24.11.2012
 */


#include <iostream>

#include "TagCreator.hpp"
#include "TagStructure.hpp"


namespace HtmlParser {

    TagStructure::TagStructure(
        BaseTag *parent_tag, const LibTree &tree, const LibTreeIter &parent_it)
    {
        std::uint32_t cnum = tree.number_of_children(parent_it);

        for (std::uint32_t i = 0; i < cnum; i++) {
            LibTreeIter it = tree.child(parent_it, i);

            if (not it->isComment() and it->isTag()) {
                it->parseAttributes();
                std::string text;

                if (it->text().find(std::string("<") + it->tagName()) == std::string::npos) {
                    text = it->text();
                }

                BaseTagCreator* tag_creator = TagCreatorBuffer::get(it->tagName());
                PTag new_tag((*tag_creator)(parent_tag, it->tagName(), it->attributes(), text));

                if (tree.number_of_children(it)) {
                    TagObjs child_tags = TagStructure(new_tag.get(), tree, it);
                    new_tag->childs(child_tags);
                }
                _sequence.push_back(new_tag);
                (*new_tag)();
            }
        }
    }


    TagStructure::operator TagObjs () {
        return _sequence;
    }
} // namespace HtmlParser
