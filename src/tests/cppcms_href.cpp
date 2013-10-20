// g++ cppcms_href.cpp -lcppcms


#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/http_file.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>
#include <iostream>
#include <stdlib.h>


class Href_a :
    public cppcms::application
{
public:
    Href_a(cppcms::service &srv)
        : cppcms::application(srv)
    {
        dispatcher().assign("", &Href_a::describe, this);
        mapper().assign("");
    }

    void describe() {
        response().out() <<
            "<html>\n"
            "   <head>\n"
            "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
            "      <title>Страничка А</title>\n"
            "   </head>\n"
            "   <body>\n"
            "       <h1>Страничка А.</h1>\n"
            "       <div class=\"Header\">\n"
            "           <div class=\"href A\"><a href=\"./href_a.html\"></a></div>\n"
            "           <div class=\"href B\"><a href=\"./href_b.html\"></a></div>\n"
            "       </div>\n"
            "   </body>\n"
            "</html>\n";
    }
};


class Href_b :
    public cppcms::application
{
public:
    Href_b(cppcms::service &srv)
        : cppcms::application(srv)
    {
        dispatcher().assign("", &Href_b::describe, this);
        mapper().assign("");
    }

    void describe() {
        response().out() <<
            "<html>\n"
            "   <head>\n"
            "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
            "      <title>Страничка Б</title>\n"
            "   </head>\n"
            "   <body>\n"
            "       <h1>Страничка Б.</h1>\n"
            "       <div class=\"Header\">\n"
            "           <div class=\"Logo\"><a href=\"./index.html\"></a></div>\n"
            "           <div class=\"Child B\"><a href=\"./href_b.html\"></a></div>\n"
            "       </div>\n"
            "   </body>\n"
            "</html>\n";
    }
};


class Index :
    public cppcms::application
{
public:
    Index(cppcms::service &srv)
        : cppcms::application(srv)
    {
        content::upload c;

        if(request().request_method()=="POST") {
            c.info.load(context());

            if(c.info.validate()) {
                std::string new_name = "latest_image";
                if(c.info.image.value()->mime() == "image/png")
                    new_name += ".png";
                else
                    new_name += ".jpg"; // we had validated mime-type
                c.info.image.value()->save_to("./uploads/" + new_name);
                c.info.clear();
            }
        }
        render("upload", c);

        dispatcher().assign("", &Index::describe, this);
        mapper().assign("");
    }

    void describe() {
        response().out() <<
            "<html>\n"
            "   <head>\n"
            "      <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n"
            "      <title>Web Сервер</title>\n"
            "   </head>\n"
            "   <body>\n"
            "       <h1>Web Сервер предосталяющий сервис на базе готовой вёрстки.</h1>\n"
            "       <div class=\"Header\">\n"
            "           <div class=\"logo\"><a href=\"./index.html\"></a></div>\n"
            "           <div class=\"href A\"><a href=\"./href_a.html\"></a></div>\n"
            "       </div>\n"
            "   </body>\n"
            "</html>\n";
    }
};


class MyApp:
    public cppcms::application
{
public:
    MyApp(cppcms::service &srv) :
        cppcms::application(srv)
    {
        attach(
            new Index(srv),
            "index",
            "./index{1}",
            "/index((.*))?",
            1);   // dispatching
        attach(
            new Href_a(srv),
            "href_a",
            "./href_a{1}",
            "/href_a((.*))?",
            1);   // dispatching
        attach(
            new Href_b(srv),
            "href_b",
            "./href_b{1}",
            "/href_b((.*))?",
            1);

        dispatcher().assign("", &MyApp::dispatch, this);
        mapper().assign("");
        mapper().root("");
    }

    void dispatch()
    {
        response().out() <<
            "<a href=\"./index\">Index</a><br>"
            "<a href=\"./href_a\">Href A</a><br>"
            "<a href=\"./href_b\">Href B</a><br>";
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
