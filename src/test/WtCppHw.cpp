/*!
 * \brief  Пример простого web приложения.
 * \author Rostislav Velichko. rostislav.vel@gmail.com
 * \date   21.1.2013
 */


// g++ WtCppHw.cpp -std=c++0x -I/usr/local/include/ -L/usr/local/lib/ -lwt -lwthttp -lboost_signals
// ./a.out --docroot . --http-address 0.0.0.0 --http-port 8080


#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>


using namespace Wt;


class HelloApplication
    : public WApplication
{
public:
  HelloApplication(const WEnvironment& env);

private:
  WText *greeting_;
};


HelloApplication::HelloApplication(const WEnvironment& env)
    : WApplication(env)
{
    setTitle("Hello world");                               // application title

    root()->addWidget(new Wt::WText(Wt::WString::fromUTF8(
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
        ));
}


WApplication *createApplication(const WEnvironment& env) {
    return new HelloApplication(env);
}


int main(int argc, char **argv) {
    return WRun(argc, argv, &createApplication);
}
