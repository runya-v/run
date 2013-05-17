/*!
 * \brief  Базовый класс отображения.

 * \author Rostislav Velichko.
 * \date   15.06.2011
 */


#pragma once


#include <Wt/WScrollArea>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WVBoxLayout>
#include <Wt/WContainerWidget>


namespace web {

    class ResultView {
    public:
        template<class T>
        static void Init(Wt::WContainerWidget *widget, T *obj, const std::string &result) {
            widget->clear();
            widget->resize(Wt::WLength(100, Wt::WLength::Percentage), Wt::WLength(100, Wt::WLength::Percentage));

            Wt::WScrollArea *sarea = new Wt::WScrollArea();
            sarea->setWidget(new Wt::WText(Wt::WString::fromUTF8(result), Wt::XHTMLUnsafeText));
            sarea->setScrollBarPolicy(Wt::WScrollArea::ScrollBarAsNeeded);
            sarea->resize(Wt::WLength(100, Wt::WLength::Percentage), Wt::WLength(100, Wt::WLength::Percentage));

            Wt::WPushButton *bt_new_action = new Wt::WPushButton(Wt::WString::tr("NewAction"));
            bt_new_action->clicked().connect(obj, &T::Init);

            Wt::WVBoxLayout *vlayout = new Wt::WVBoxLayout(widget);
            vlayout->addWidget(new Wt::WText(Wt::WString::tr("ResultView")), 0);
            vlayout->addWidget(sarea, 1);
            vlayout->addWidget(bt_new_action, 0);
        }
    };
}
