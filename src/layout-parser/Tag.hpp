/*!
 * \brief  Класс HTML Тэг.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   23.11.2012
 */


#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>


namespace HtmlParser {

    class BaseTag;

    typedef std::map< std::string, std::string > LibArrts;
    typedef LibArrts::iterator                   LibArrtIter;
    typedef LibArrts::value_type                 LibArrtValue;
    typedef std::shared_ptr< BaseTag >           PTag;
    typedef std::vector< PTag >                  TagObjs;
    typedef TagObjs::iterator                    TagObjsIter;


    class BaseTag {
    protected:
        BaseTag *_parent;
        std::string _name;
        LibArrts _attrs;
        std::string _text;
        TagObjs _childs;
        std::string _shift;

    public:
        static std::string attributesText(const LibArrts &attrs);

        BaseTag(
            BaseTag *parent,
            const std::string &name,
            const LibArrts &attrs,
            const std::string &text
            );

        virtual ~BaseTag();

        virtual void operator() ();
        virtual operator std::string ();

        BaseTag* parent();
        void childs(const TagObjs &chs);
        const TagObjs& childs();
    };


    class UndefinedTag
        : public BaseTag
    {
    public:
        UndefinedTag(
            BaseTag *parent,
            const std::string &name,
            const LibArrts &attrs,
            const std::string &text
            );
    };
} // namespace HtmlParser
