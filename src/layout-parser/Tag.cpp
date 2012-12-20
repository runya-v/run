/*!
 * \brief  Класс HTML Тэг.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   23.11.2012
 */


#include <memory>
#include <vector>
#include <iostream>
#include <sstream>

#include "TagCreator.hpp"
#include "Tree.hpp"
#include "Tag.hpp"


namespace HtmlParser {

    std::string BaseTag::attributesText(const LibArrts &attrs) {
        std::string str;

        for (const LibArrtValue &value : attrs) {
            if (not value.first.empty() and not value.second.empty()) {
                str += " " + value.first + "=" + value.second;
            }
        }
        return str;
    }


    BaseTag::BaseTag(
        BaseTag *parent,
        const std::string &name,
        const LibArrts &attrs,
        const std::string &text)
        : _parent(parent)
        , _name(name)
        , _attrs(attrs)
        , _text(text)
    {}


    BaseTag::~BaseTag()
    {}


    void BaseTag::operator() () {
        std::clog << "Tag `" << _name << "`:\n";
        std::clog << "attrs: {\n" << attributesText(_attrs) << "}\n";

        if (not _text.empty()) {
            std::clog << "text: {\n" << _text << "\n}\n";
        }

        if (not _childs.empty()) {
            std::clog << "childs: {\n" << _childs.size() << "\n}\n";
        }
    }


    BaseTag::operator std::string () {
        std::stringstream ss;
        std::string attrs = attributesText(_attrs);

        if (_parent) {
            _shift = _parent->_shift + " ";
        }

        if (not _text.empty()) {
            ss << _shift << "<" << _name << attrs + ">" << _text << "</" << _name << ">";
        }
        else if (not _childs.empty()) {
            ss << _shift << "<" << _name << attrs + ">\n";

            for (PTag& tag : _childs) {
                ss << std::string(*tag);
            }
            ss << _shift << "</" << _name << ">";
        }
        else {
            ss << _shift << "<" << _name << attrs + "/>";
        }
        ss << "\n";

        return ss.str();
    }


    BaseTag* BaseTag::parent() {
        return _parent;
    }


    void BaseTag::childs(const TagObjs &c) {
        _childs.insert(_childs.begin(), c.begin(), c.end());
    }


    const TagObjs& BaseTag::childs() {
        return _childs;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////


    UndefinedTag::UndefinedTag(
        BaseTag *parent,
        const std::string &name,
        const LibArrts &attrs,
        const std::string &text)
        : BaseTag(parent, name, attrs, text)
    {}
} // namespace HtmlParser
