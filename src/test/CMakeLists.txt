#add_library("test" SHARED ${MAIN_SRC_DIR}/Plugin.cpp Test.cpp)

include_directories(${HTMLCXX_DIR})
add_library("parse_path" SHARED ${MAIN_SRC_DIR}/Plugin.cpp ParsePath.cpp)

include_directories(${HTMLCXX_DIR})
add_library("html_parser" SHARED ${MAIN_SRC_DIR}/Plugin.cpp HtmlParser.cpp )
target_link_libraries("html_parser" htmlcxx)
