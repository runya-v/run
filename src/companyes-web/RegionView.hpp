/*!
 * \brief  Класс отображения меню регионов с компаниями.
 * \author Rostislav Velichko.
 * \date   01.30.2013
 */

#pragma once

#include <Wt/WContainerWidget>
#include <Wt/WStackedWidget>
#include <Wt/WMenu>


namespace Web {

    class RegionView
        : public Wt::WContainerWidget
    {
        Wt::WStackedWidget *_parent;

    public:
        /*!
         * \brief Конструктор класса отображения заданного региона.
         * \param[in] Имя директории с регионом
         * \param[in] Виджет для вывода содержимого файла рубрики
         */
        RegionView(const std::string &abs_path_name, Wt::WStackedWidget *contents, Wt::WMenu *parent_menu);
    };
}
