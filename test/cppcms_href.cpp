// g++ cppcms_href.cpp -lcppcms


#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>
#include <iostream>
#include <stdlib.h>


class Href_a :
    public cppcms::application
{
    Href_a(cppcms::service &srv) 
        : cppcms::application(srv) 
    {}

    void init() {
        response().out() <<
            "<html>\n"
            "   <head>\n"
            "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
            "      <title>Страничка А</title>\n"
            "   </head>\n"
            "   <body>\n"
            "       <h1>Web Сервер предосталяющий сервис на базе готовой вёрстки.</h1>\n"
            "       <div class=\"href A\"><a href=\"./href_a.html\"></a></div>"
            "       <div class=\"href B\"><a href=\"./href_b.html\"></a></div>"
            "   </body>\n"
            "</html>\n";
    }
};


class Href_b :
    public cppcms::application
{
    Href_b(cppcms::service &srv) 
        : cppcms::application(srv) 
    {}
    
    void init() {
        response().out() <<
            "<html>\n"
            "   <head>\n"
            "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
            "      <title>Страничка А</title>\n"
            "   </head>\n"
            "   <body>\n"
            "       <h1>Страничка А.</h1>\n"
            "       <div class=\"logo\"><a href=\"./index.html\"></a></div>"
            "       <div class=\"href B\"><a href=\"./href_b.html\"></a></div>"
            "   </body>\n"
            "</html>\n";
    }
};


class Index :
    public cppcms::application
{
    Index(cppcms::service &srv) 
        : cppcms::application(srv) 
    {}

    void init() {
        response().out() <<
            "<html>\n"
            "   <head>\n"
            "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
            "      <title>Страничка Б</title>\n"
            "   </head>\n"
            "   <body>\n"
            "       <h1>Страничка Б.</h1>\n"
            "       <div class=\"logo\"><a href=\"./index.html\"></a></div>"
            "       <div class=\"href A\"><a href=\"./href_a.html\"></a></div>"
            "   </body>\n"
            "</html>\n";
    }
};


//class numbers :
//    public cppcms::application
//{
//public:
//    numbers(cppcms::service &srv) : cppcms::application(srv)
//    {
//        dispatcher().assign("",&numbers::all,this);
//        mapper().assign("");
//
//        dispatcher().assign("/odd",&numbers::odd,this);
//        mapper().assign("odd","/odd");
//
//        dispatcher().assign("/even",&numbers::even,this);
//        mapper().assign("even","/even");
//
//        dispatcher().assign("/prime",&numbers::prime,this);
//        mapper().assign("prime","/prime");
//    }
//
//    void all()
//    {
//        response().out()
//            << "<a href='" << url("/")       << "'>Top</a><br>"
//            << "<a href='" << url("/letters")<< "'>Letters</a><br>"
//            << "<a href='" << url(".")       << "'>All Numbers</a><br>"
//            << "<a href='" << url("odd")     << "'>Odd Numbers</a><br>"
//            << "<a href='" << url("even")    << "'>Even Numbers</a><br>"
//            << "<a href='" << url("prime")   << "'>Prime Numbers</a><br>"
//            << "1,2,3,4,5,6,7,8,9,10,...";
//    }
//
//    void prime()
//    {
//        response().out() << "2,3,5,7,...";
//    }
//    void odd()
//    {
//        response().out() << "1,3,5,7,9,...";
//    }
//    void even()
//    {
//        response().out() << "2,4,6,8,10,...";
//    }
//};
//
//
//class letters :
//    public cppcms::application
//{
//public:
//    letters(cppcms::service &srv) : cppcms::application(srv)
//    {
//        dispatcher().assign("",&letters::all,this);
//        mapper().assign("");
//
//        dispatcher().assign("/capital",&letters::capital,this);
//        mapper().assign("capital","/capital");
//
//        dispatcher().assign("/small",&letters::small,this);
//        mapper().assign("small","/small");
//
//    }
//
//    void all()
//    {
//        response().out()
//            << "<a href=\"/\">Top</a><br>"
//            << "<a href=\"/numbers\">Numbers</a><br>"
//            << "<a href=\"/letters\">All Letters</a><br>"
//            << "<a href=\"/letters/capital\">Capital Letters</a><br>"
//            << "<a href=\"/letters/small\">Small Letters</a><br>"
//            << "Aa, Bb, Cc, Dd,...";
//    }
//
//    void capital()
//    {
//        response().out() << "A,B,C,D,...";
//    }
//    void small()
//    {
//        response().out() << "a,b,c,d,...";
//    }
//};


class MyApp:
    public cppcms::application
{
public:
    myapp(cppcms::service &srv) :
        cppcms::application(srv)
    {
        attach(
            new Index(srv),
            "index.html",
            "./index.html{1}",
            "/index.html((.*))?",
            1);   // dispatching
        attach(
            new Href_a(srv),
            "href_a.html",
            "./href_a.html{1}",
            "/href_a.html((.*))?",
            1);   // dispatching
        attach(
            new Href_a(srv),
            "href_b.html",
            "./href_b.html{1}",
            "/href_b.html((.*))?",
            1);

        dispatcher().assign("", &MyApp::init, this);
        mapper().assign(""); // default URL

        mapper().root("");
    }

    void init()
    {
        response().out()
            << "<a href=\"./index.html\">Index</a><br>"
            << "<a href=\"./href_a.html\">Href A</a><br>"
            << "<a href=\"./href_b.html\">Href B</a><br>";
    }
};


int main(int argc, char **argv) {
    try {
        cppcms::service app(argc,argv);
        app.applications_pool().mount(cppcms::applications_factory< MyApp >());
        app.run();
    }
    catch(std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
}
