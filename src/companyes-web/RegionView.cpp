#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <Wt/WString>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WLength>
#include <Wt/WLabel>
#include <Wt/WPushButton>
#include <Wt/WSubMenuItem>

#include "RubrickWidget.hpp"
#include "SignaledMenuItem.hpp"
#include "RegionView.hpp"


namespace Web {

    namespace bfs = boost::filesystem;


    RegionView::RegionView(const std::string &abs_path_name, Wt::WStackedWidget *contents, Wt::WMenu *parent_menu)
        : _parent(contents)
    {
        Wt::WMenu *menu = new Wt::WMenu(contents, Wt::Vertical, 0);
        menu->setRenderAsList(true);
        menu->setStyleClass("menu submenu");

        bfs::path abs_path = bfs::path(abs_path_name);

        if (bfs::exists(abs_path)) {
            bfs::directory_iterator it_dir(abs_path);
            bfs::directory_iterator eod;

            BOOST_FOREACH(const bfs::path &region_path, std::make_pair(it_dir, eod)) {
                if (bfs::is_regular_file(region_path)) {
                    RubrickWidget *widget = new RubrickWidget(region_path.string(), this);
                    SignaledMenuItem *item = new SignaledMenuItem(Wt::WString::fromUTF8(region_path.stem().string()), widget);
                    menu->addItem(item);
                    item->activateSignal().connect(widget, &RubrickWidget::fullContent);
                }
            }
        }

        Wt::WSubMenuItem *menu_item = new Wt::WSubMenuItem(Wt::WString::fromUTF8(abs_path.stem().string()), this);
        menu_item->setSubMenu(menu);
        parent_menu->addItem(menu_item);
    }
}
