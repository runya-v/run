#include <iostream>

#include <boost/filesystem.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WApplication>
#include <Wt/WProgressBar>
#include <Wt/WComboBox>
#include <Wt/WText>
#include <Wt/WString>
#include <Wt/WMenu>
#include <Wt/WLocalizedStrings>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WOverlayLoadingIndicator>

#include "Cript.hpp"
#include "UnCript.hpp"
#include "CriptView.hpp"


namespace Web {

    namespace bfs = boost::filesystem;


    class PleyfairApplication : public Wt::WApplication {
    public:
        /*!
         * \brief Конструктор класса сайта.
         * \param[in] Класс хранения окружения сайта
         */
        PleyfairApplication(const Wt::WEnvironment& env)
            : Wt::WApplication(env)
        {
            // Применение локализации
            this->messageResourceBundle().use(this->appRoot() + "rus_locale");

            // Применене web стилей
            this->useStyleSheet(this->appRoot() + "main.css");
            this->setTitle(Wt::WString::tr("Title"));

            Wt::WApplication *app = Wt::WApplication::instance();
            app->setLoadingIndicator(new Wt::WOverlayLoadingIndicator());
            app->styleSheet().addRule("body", "margin: 0px");

            Wt::WStackedWidget *content = new Wt::WStackedWidget();
            content->setOverflow(Wt::WContainerWidget::OverflowAuto);
            content->setPositionScheme(Wt::Relative);

            Wt::WMenu *menu = new Wt::WMenu(content, Wt::Vertical, NULL);
            menu->setRenderAsList(true);
            menu->setStyleClass("menu");

            std::string absolute_path = bfs::system_complete(bfs::path(".")).string();
            menu->addItem(Wt::WString::tr("Cript"), new CriptView<Cript>(content, absolute_path));
            menu->addItem(Wt::WString::tr("UnCript"), new CriptView<UnCript>(content, absolute_path));

            Wt::WHBoxLayout *hlayout = new Wt::WHBoxLayout();
            hlayout->addWidget(menu, 0);
            hlayout->addWidget(content, 1);

            Wt::WVBoxLayout *vlayout = new Wt::WVBoxLayout(this->root());
            vlayout->addWidget(new Wt::WText(Wt::WString::tr("Header")), 0);
            vlayout->addLayout(hlayout, 1);
        }
    };


    //! \ brief Функция генерации web приложения
    Wt::WApplication* CreateApp(const Wt::WEnvironment& env) {
        return new PleyfairApplication(env);
    }
}


// Пример запуска: ./Pleyfair --docroot . --http-address 0.0.0.0 --http-port=8080
int main(int argc, char **argv) {
    return Wt::WRun(argc, argv, Web::CreateApp);
}
