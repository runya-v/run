#include "SignaledMenuItem.hpp"


namespace Web {

    SignaledMenuItem::SignaledMenuItem(const Wt::WString &text, Wt::WWidget *contents, LoadPolicy policy)
        : Wt::WSubMenuItem(text, contents, policy)
    {}


    Wt::SignalBase& SignaledMenuItem::activateSignal() {
        return Wt::WSubMenuItem::activateSignal();
    }
}
