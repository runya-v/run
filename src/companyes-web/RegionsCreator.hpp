/*!
 * \brief  Класс - генератор пунктов бокового меню для каждой рубрики компаний.
 * \author Rostislav Velichko.
 * \date   01.30.2013
 */

#pragma once

#include <string>

#include <Wt/WStackedWidget>
#include <Wt/WMenu>

#include "RegionView.hpp"


namespace Web {

    class RegionsCreator {
    public:
        RegionsCreator(const std::string &path, Wt::WStackedWidget *content, Wt::WMenu *menu);
    };
}
