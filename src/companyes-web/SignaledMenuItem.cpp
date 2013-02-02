#include "SignaledMenuItem.hpp"


namespace Web {

    SignaledMenuItem::SignaledMenuItem(const Wt::WString &text, Wt::WWidget *contents, LoadPolicy policy)
        : Wt::WMenuItem(text, contents, policy)
    {}


    Wt::SignalBase& SignaledMenuItem::activateSignal() {
        return Wt::WMenuItem::activateSignal();
    }
}
