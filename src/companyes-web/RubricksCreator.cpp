#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include "RubricksCreator.hpp"


namespace Web {

    namespace bfs = boost::filesystem;


    RubricksCreator::RubricksCreator(const std::string &path, Wt::WStackedWidget *content, Wt::WMenu *menu)
    {
        bfs::path abs_path = bfs::absolute(bfs::path(path));

        if (bfs::exists(abs_path)) {
            bfs::directory_iterator it_dir(abs_path);
            bfs::directory_iterator eod;

            BOOST_FOREACH(const bfs::path &it, std::make_pair(it_dir, eod)) {
                std::cout << it.string() << "\n";
            }
        }
    }
}
