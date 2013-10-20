#include <iostream>

#include <boost/spirit/core.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/attribute.hpp>
#include <boost/fusion/tuple.hpp>

namespace bf = boost::fusion;
namespace bs = boost::spirit;
namespace qi = bs::qi;


//struct MyGrammar
//    : public spr::grammar<MyGrammar>
//{
//    template<typename Scanner>
//    struct definition {
//        spr::rule<Scanner> wordGroup, word;

//        definition(const MyGrammar &self) {
//            wordGroup = '[' >> *( word | wordGroup ) >> ']';
//            word = lexeme_d[ +chset_p("a-zA-Z") ];
//        }

//        boost::spirit::rule<ScannerT> const & start() const {
//            return wordGroup;
//        }
//    };
//};

struct MyGrammar
    : qi::grammar<std::string::iterator>
{
    calculator_simple()
        : calculator_simple::base_type(expression)
    {
        qi::lit("CMD\r\n") >>
        +(qi::alnum - '=') >> qi::lit("=") >> +(qi::alnum - ' ') >> qi::lit(" ") >>
        +(qi::alnum - '=') >> qi::lit("=") >> +(qi::alnum - '\r' - '\n') >> qi::lit("\r\n") >>
        qi::lit(".\r\n")
    }

    qi::rule<std::string::iterator> expression, term, factor;
};


static std::string DATA = "CMD\r\nvalue1=123 value2=333\r\n.\r\n";

int main() {
    bf::tuple<std::string, std::string> t;
    qi::rule<std::string::iterator> q = (
        qi::lit("CMD\r\n") >>
        +(qi::alnum - '=') >> qi::lit("=") >> +(qi::alnum - ' ') >> qi::lit(" ") >>
        +(qi::alnum - '=') >> qi::lit("=") >> +(qi::alnum - '\r' - '\n') >> qi::lit("\r\n") >>
        qi::lit(".\r\n")
    );

    //qi::parse(DATA.begin(), DATA.end(), q, t);
    qi::parse(DATA.begin(), DATA.end(), (
                  qi::lit("CMD\r\n") >>
                  +(qi::alnum - '=') >> qi::lit("=") >> +(qi::alnum - ' ') >> qi::lit(" ") >>
                  +(qi::alnum - '=') >> qi::lit("=") >> +(qi::alnum - '\r' - '\n') >> qi::lit("\r\n") >>
                  qi::lit(".\r\n")
              ), t);

    std::cout << bf::get<0>(t) << "=" << bf::get<1>(t) << "\n";
    return 0;
}

//typedef boost::fusion::tuple<
//std::string,
//std::string,
//std::string,
//std::string,
//std::string,
//std::string,
//std::string,
//std::string,
//std::string
//> MyTuple;

//MyTuple tp


//auto q_quote = (
//            +(q::digit) >> q::lit(" ") >>
//            (q::alpha) >> q::lit(" ") >>
//            +(q::alpha | (q::punct - ' ')) >> q::lit(" ") >>
//            (q::alnum) >> q::lit(" ") >>
//            +(q::digit | (q::punct - ' ')) >> q::lit(" ") >>
//            +(q::digit | (q::punct - ' ')) >> q::lit(" ") >>
//            +(q::digit | (q::punct - ' ')) >> q::lit(" ") >>
//            +(q::digit) >> q::lit("\r\n")
//        );

//std::string data("12312 f safas s 231 24 342 234 243\r\n");

//    bool success = q::parse(
//        data.begin(), data.end(),
//        q_quote,
//        tp
//    );


