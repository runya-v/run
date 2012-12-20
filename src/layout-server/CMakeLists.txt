set(PLUGIN "layout_server")

add_library(${PLUGIN} SHARED
    ${MAIN_SRC_DIR}/Plugin.cpp
    HtmlTree.cpp
    HtmlParser.cpp
    Server.cpp
    Main.cpp
    )

include_directories("/usr/local/include/htmlcxx")

target_link_libraries(${PLUGIN}
    ${PLUGIN}
    cppcms
    htmlcxx
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )
