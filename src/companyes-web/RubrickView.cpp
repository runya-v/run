#include <Wt/WString>
#include <Wt/WTable>
#include <Wt/WLength>
#include <Wt/WLabel>
#include <Wt/WPushButton>

#include "RubrickView.hpp"


namespace Web {

    RubrickView::RubrickView(const std::string &path, Wt::WStackedWidget *contents)
        : _parent(contents)
    {
        this->clear();

        Wt::WTable* table = new Wt::WTable(this);
        table->setStyleClass("stamper.table");

        new Wt::WLabel(Wt::WString::tr("StamperView.InputBaseName"), table->elementAt(0, 0));
        table->elementAt(0, 0)->resize(Wt::WLength::Auto, Wt::WLength::Auto);

        Wt::WPushButton* bt_next = new Wt::WPushButton(Wt::WString("Следующие 10").toUTF8(), table->elementAt(1, 1));
        bt_next->clicked().connect(this, &RubrickView::onButtonNext);

        Wt::WPushButton* bt_old = new Wt::WPushButton(Wt::WString("Предыдущие 10").toUTF8(), table->elementAt(1, 1));
        bt_old->clicked().connect(this, &RubrickView::onButtonOld);
    }

    void RubrickView::onButtonNext() {

    }


    void RubrickView::onButtonOld() {

    }
}
