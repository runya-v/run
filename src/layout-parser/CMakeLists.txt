set(PLUGIN "layout_parser")

add_library(${PLUGIN} SHARED
    ${MAIN_SRC_DIR}/Plugin.cpp
    #Main.cpp
    Tag.cpp
    Tree.cpp
    TagCreator.cpp
    TagStructure.cpp
    LayoutParser.cpp
    )

include_directories(
    ${CPPCMS_DIR} 
    ${HTMLCXX_DIR}
    )

target_link_libraries(
    ${PLUGIN}
    dl
    ${CPPCMS_LIBRARY}
    ${HTMLCXX_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )

#set(PLUGIN_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PLUGIN}")

#add_custom_command(
#    TARGET ${PLUGIN} POST_BUILD
#    COMMAND bash -c "if [ -d ${PLUGIN_DIR} ]\; then rm -r ${PLUGIN_DIR}\; fi\; mkdir ${PLUGIN_DIR}"
#    COMMAND cp "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run" "${PLUGIN_DIR}"
#    COMMAND cp "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PLUGIN}.so" "${PLUGIN_DIR}"
#    DEPENDS ${RUN_BIN} ${PLUGIN}
#    )
