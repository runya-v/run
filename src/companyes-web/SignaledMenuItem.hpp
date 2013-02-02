/*!
 * \brief  Класс пункта меню, предоставляющий доступ к сигналу выбора данного пункта.
 * \author Rostislav Velichko.
 * \date   02.02.2013
 */

#pragma once

#include <Wt/WWidget>
#include <Wt/WMenuItem>
#include <Wt/WString>


namespace Web {

    class SignaledMenuItem
        : public Wt::WMenuItem
    {
    public:
        SignaledMenuItem(const Wt::WString &text, Wt::WWidget *contents, LoadPolicy policy = Wt::WMenuItem::LazyLoading);

        Wt::SignalBase& activateSignal();
    };
}
