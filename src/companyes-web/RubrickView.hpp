/*!
 * \brief  Класс отображения компаний рубрики.
 * \author Rostislav Velichko.
 * \date   01.30.2013
 */

#pragma once

#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>


namespace Web {

    class RubrickView
        : public Wt::WContainerWidget
    {
        Wt::WStackedWidget *_parent;

    public:
        /*!
         * \brief Конструктор класса отображения заданной рубрики.
         * \param[in] Имя директории рубрики
         */
        RubrickView(const std::string &path, Wt::WStackedWidget *contents);

        void onButtonNext();
        void onButtonOld();
    };
}
