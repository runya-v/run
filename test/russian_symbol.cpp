#include <boost/regex.hpp>
#include <string>
#include <iostream>


std::string correctRegionName(const std::string& name) {
    if (regex_match(name, boost::regex("[А-Яа-яЁё].*"))) {
        return std::string(name, 0, name.find(','));
    }
    else {
        return name.substr(name.rfind(',') + 2);
    }
}


int main() {
    std::cout << "`" << correctRegionName("Москва, Ленинградский проспект 12") << "`\n";
    std::cout << "`" << correctRegionName("Leningradskii prospect 12, Moskov") << "`\n";
    std::cout << "`" << correctRegionName("Moskov, Ленинградский проспект 12") << "`\n";
    return 0;
}
