#add_library("test" SHARED ${MAIN_SRC_DIR}/Plugin.cpp Test.cpp)

#include_directories(${HTMLCXX_DIR})
#add_library("parse_path" SHARED ${MAIN_SRC_DIR}/Plugin.cpp ParsePath.cpp)

#include_directories(${HTMLCXX_DIR})
#add_library("html_parser" SHARED ${MAIN_SRC_DIR}/Plugin.cpp HtmlParser.cpp )
#target_link_libraries("html_parser" htmlcxx)

add_executable(spirit_grammar spirit_grammar.cpp)
target_link_libraries(spirit_grammar
    pthread
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    )

