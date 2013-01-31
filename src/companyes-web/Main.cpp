/*!
 * \brief  Web приложение предоставляющее удобное отображение компаний.
 * \author Rostislav Velichko.
 * \date   01.30.2013
 */


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

#include "RubricksCreator.hpp"


namespace Web {

    class CompanyesApplication : public Wt::WApplication {
    public:
        /*!
         * \brief Конструктор класса сайта.
         * \param[in] Класс хранения окружения сайта
         */
        CompanyesApplication(const Wt::WEnvironment& env)
            : Wt::WApplication(env)
        {
            std::string absolute_path = boost::filesystem::system_complete(boost::filesystem::path(".")).string();

            this->messageResourceBundle().use(this->appRoot() + "rus_locale");
             this->useStyleSheet(this->appRoot() + "main.css");
            this->setTitle(Wt::WString::tr("Title"));

            Wt::WApplication *app = Wt::WApplication::instance();
            app->setLoadingIndicator(new Wt::WOverlayLoadingIndicator());
            app->styleSheet().addRule("body", "margin: 0px");

            Wt::WStackedWidget* contents = new Wt::WStackedWidget();
            contents->setOverflow(Wt::WContainerWidget::OverflowAuto);
            contents->setPositionScheme(Wt::Relative);

            Wt::WMenu* menu = new Wt::WMenu(contents, Wt::Vertical, 0);
            menu->setRenderAsList(true);
            menu->setStyleClass("menu");

            new RubricksCreator(absolute_path, contents, menu);

            Wt::WHBoxLayout* hlayout = new Wt::WHBoxLayout();
            hlayout->addWidget(menu, 0);
            hlayout->addWidget(contents, 1);

            Wt::WVBoxLayout* vlayout = new Wt::WVBoxLayout(this->root());
            vlayout->addWidget(new Wt::WText(Wt::WString::tr("Header")), 0);
            vlayout->addLayout(hlayout, 1);
        }
    };


    //! \ brief Функция генерации web приложения
    Wt::WApplication* CreateApp(const Wt::WEnvironment& env) {
        return new CompanyesApplication(env);
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Пример запуска: ./companyes-web --docroot . --http-address 0.0.0.0 --http-port=8080
int main(int argc, char **argv) {
    return Wt::WRun(argc, argv, Web::CreateApp);
}
