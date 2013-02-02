/*!
 * \brief  Класс отображения рубрик компаниями.
 * \author Rostislav Velichko.
 * \date   01.31.2013
 */

#pragma once

#include <Wt/WContainerWidget>
#include <Wt/WString>

#include <string>


namespace Web {

    class RubrickWidget
         : public Wt::WContainerWidget
    {
        std::string _file_name;
        Wt::WString _content;

    public:
        RubrickWidget(const std::string &file_name, Wt::WContainerWidget *parent);

        void fullContent();
    };
}
