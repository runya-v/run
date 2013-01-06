set(LIBXML2_LIBRARIES ${LIBXML2_LIBRARY})
set(LIBXML2_INCLUDE_DIRS ${LIBXML2_INCLUDE_DIR})

set(PLUGIN "layout_server")

add_library(${PLUGIN} SHARED
    ${MAIN_SRC_DIR}/Plugin.cpp
    HtmlTree.cpp
    HtmlParser.cpp
    HtmlHref.cpp
    Server.cpp
    Main.cpp
    )

include_directories(
    ${CPPCMS_DIR} 
    ${HTMLCXX_DIR}
    )

target_link_libraries(${PLUGIN}
    ${PLUGIN}
    ${CPPCMS_LIBRARY}
    ${HTMLCXX_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )
