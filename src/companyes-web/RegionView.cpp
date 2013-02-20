#include <vector>
#include <string>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

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
    typedef Lines::value_type LinesValue;
    typedef std::map<int, Lines> Items;
    typedef Items::iterator ItemIter;
    typedef Items::value_type ItemValue;


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
                        int num = boost::lexical_cast< std::string >(lines[lines.size()-1]);

                        ItemIter it = items.find(num);

                        if (it not_eq items.end()) {
                            it.second.push_back(region_path.stem().string())
                        }
                        else {
                            lines.clear();
                            lines.push_back(region_path.stem().string());
                            items[num] = lines;
                        }
                    }
                }
            }

            for (const ItemValue& item : Items) {
                for (const LinesValue& line : item.second) {
                    RubrickWidget *widget = new RubrickWidget(line, this);
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
