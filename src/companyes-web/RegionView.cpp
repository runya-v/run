#include <vector>
#include <string>
#include <map>
#include <limits>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

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
    namespace ba = boost::algorithm;

    typedef std::vector<std::string> Lines;
    typedef std::map<int, Lines> Items;
    typedef Items::iterator ItemIter;


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

            Items items;

            BOOST_FOREACH(const bfs::path &region_path, std::make_pair(it_dir, eod)) {
                if (bfs::is_regular_file(region_path)) {
                    Lines lines;
                    ba::split(lines, region_path.stem().string(), ba::is_any_of("-."), ba::token_compress_on);

                    if (lines.size() >= 2) {
                        std::string str_num = lines[lines.size()-1];
                        int num = std::numeric_limits<int>::max() - boost::lexical_cast< int >(str_num);

                        ItemIter it = items.find(num);

                        if (it not_eq items.end()) {
                            it->second.push_back(region_path.string());
                        }
                        else {
                            lines.clear();
                            lines.push_back(region_path.string());
                            items[num] = lines;
                        }
                    }
                }
            }

            for (auto item : items) {
                for (auto line : item.second) {
                    RubrickWidget *widget = new RubrickWidget(line, this);
                    bfs::path region_path(line);
                    SignaledMenuItem *menu_item = new SignaledMenuItem(Wt::WString::fromUTF8(region_path.stem().string()), widget);
                    menu->addItem(menu_item);
                    menu_item->activateSignal().connect(widget, &RubrickWidget::fullContent);
                }
            }
        }

        Wt::WSubMenuItem *menu_item = new Wt::WSubMenuItem(Wt::WString::fromUTF8(abs_path.stem().string()), this);
        menu_item->setSubMenu(menu);
        parent_menu->addItem(menu_item);
    }
}
