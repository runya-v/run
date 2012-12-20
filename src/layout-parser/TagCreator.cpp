/*!
 * \brief  Генератор HTML Тэга.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   23.11.2012
 */


#include "Error.hpp"
#include "TagCreator.hpp"


namespace HtmlParser {


    Creators TagCreatorBuffer::_creators_;


    struct InitCreatorBuffer {
        InitCreatorBuffer() {
            TagCreatorBuffer::init("undefined", PCreator(new TagCreator< BaseTag >()));
        }
    } init_creators_;


    BaseTagCreator::~BaseTagCreator()
    {}


    void TagCreatorBuffer::init(const std::string &tag_name, PCreator creator) {
        if (not TagCreatorBuffer::_creators_.insert(std::make_pair(tag_name, creator)).second) {
            BOOST_THROW_EXCEPTION(Error() << Error::Msg(
                std::string("Can`t insert new Tag creator by `") + tag_name + "` is exists."));
        }
    }


    BaseTagCreator* TagCreatorBuffer::get(const std::string &tag_name) {
        CreatorsIter it = _creators_.find(tag_name);

        if (it not_eq _creators_.end()) {
            return it->second.get();
        }
        else {
            it = _creators_.find("undefined");

            if (it not_eq _creators_.end()) {
                return it->second.get();
            }
            else {
                BOOST_THROW_EXCEPTION(
                    Error() << Error::Msg(std::string("Can`t find Tag creator for `undefined`.")));
            }
        }
    }
} // namespace HtmlParser
