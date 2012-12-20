#add_library("test" SHARED ${MAIN_SRC_DIR}/Plugin.cpp Test.cpp)

#add_library("parse_path" SHARED ${MAIN_SRC_DIR}/Plugin.cpp ParsePath.cpp)

include_directories("/usr/local/include/htmlcxx")
add_library("html_parser" SHARED ${MAIN_SRC_DIR}/Plugin.cpp HtmlParser.cpp)
target_link_libraries("html_parser" htmlcxx)

add_executable("test_rusymb" russian_symbol.cpp)
target_link_libraries("test_rusymb" ${Boost_REGEX_LIBRARY})
