#include <iostream>
#include <boost/filesystem.hpp>


// g++ ./bfs-decomposition.cpp -lboost_filesystem -lboost_system


int main() {
    namespace bfs = boost::filesystem;

    bfs::path path_1("/home/runya/develop/test.tmp");

    std::cout << "\npath:" << path_1 << "\n";
    std::cout << "filename:       `" << path_1.filename() << "`\n";
    std::cout << "extension:      `" << path_1.extension() << "`\n";
    std::cout << "root_name:      `" << path_1.root_name() << "`\n";
    std::cout << "root_path:      `" << path_1.root_path() << "`\n";
    std::cout << "root_directory: `" << path_1.root_directory() << "`\n";
    std::cout << "relative_path:  `" << path_1.relative_path() << "`\n";
    std::cout << "parent_path:    `" << path_1.parent_path() << "`\n";
    std::cout << "stem:           `" << path_1.stem() << "`\n";

    bfs::path path_2("../develop/test.tmp");

    std::cout << "\npath:" << path_2 << "\n";
    std::cout << "filename:       `" << path_2.filename() << "`\n";
    std::cout << "extension:      `" << path_2.extension() << "`\n";
    std::cout << "root_name:      `" << path_2.root_name() << "`\n";
    std::cout << "root_path:      `" << path_2.root_path() << "`\n";
    std::cout << "root_directory: `" << path_2.root_directory() << "`\n";
    std::cout << "relative_path:  `" << path_2.relative_path() << "`\n";
    std::cout << "parent_path:    `" << path_2.parent_path() << "`\n";
    std::cout << "stem:           `" << path_2.stem() << "`\n";

    return 0;
}
