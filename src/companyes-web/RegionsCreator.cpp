#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "RegionsCreator.hpp"


namespace Web {

    namespace bfs = boost::filesystem;

    static const char companyes_dir_[] = "companyes";


    RegionsCreator::RegionsCreator(const std::string &path, Wt::WStackedWidget *content, Wt::WMenu *menu) {
        bfs::path abs_path = bfs::absolute(bfs::path(path) / bfs::path(companyes_dir_));

        if (bfs::exists(abs_path)) {
            bfs::directory_iterator it_dir(abs_path);
            bfs::directory_iterator eod;

            BOOST_FOREACH(const bfs::path &region_path, std::make_pair(it_dir, eod)) {
                if (bfs::is_directory(region_path)) {
                    new RegionView(region_path.string(), content, menu);
                }
            }
        }
    }
}
