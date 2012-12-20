/*!
 * \brief  Генератор HTML Тэга.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   23.11.2012
 */


#pragma once

#include <memory>
#include <map>
#include <string>

#include "Tag.hpp"


namespace HtmlParser {

    class BaseTagCreator {
    public:
        virtual ~BaseTagCreator();

        virtual PTag operator() (
            BaseTag*, const std::string&, const LibArrts&, const std::string&
            ) = 0;
    };


    template< class Type >
    class TagCreator
        : public BaseTagCreator
    {
    public:
        PTag operator() (
            BaseTag *parent,
            const std::string &name,
            const LibArrts &attrs,
            const std::string &str)
        {
            return PTag(dynamic_cast< BaseTag* >(new Type(parent, name, attrs, str)));
        }
    };


    typedef std::shared_ptr< BaseTagCreator > PCreator;
    typedef std::map< std::string, PCreator > Creators;
    typedef Creators::iterator                CreatorsIter;
    typedef Creators::value_type              CreatorsValue;
    typedef std::pair< CreatorsIter, bool >   InsertValue;


    class TagCreatorBuffer {
    protected:
        static Creators _creators_;

    public:
        static void init(const std::string &tag_name, PCreator creator);
        static BaseTagCreator* get(const std::string &tag_name);
    };
} // namespace HtmlParser
