/*!
 * \brief  Класс - генератор пунктов бокового меню для каждой рубрики компаний.
 * \author Rostislav Velichko.
 * \date   01.30.2013
 */

#pragma once

#include <string>

#include <Wt/WStackedWidget>
#include <Wt/WMenu>

#include "RubrickView.hpp"

namespace Web {

    class RubricksCreator {
    public:
        RubricksCreator(const std::string &path, Wt::WStackedWidget *content, Wt::WMenu *menu);
    };
}
