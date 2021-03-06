set(PLUGIN "company_parser")

# Тип разбора компаний с учётом имеющих собственный сайт включается директивой ALL_PARSE=1

OPTION(ENABLE_ALL_PARSE "Enable TESTS support [default: ON]"   ON)


if(ENABLE_ALL_PARSE)
    message("Set cmake parse companies with url.")
    add_definitions(-DALL_PARSE)
else()
    message("If you need parse companies with url - run cmake -DALL_PARSE=1 to include.")
endif()

add_library(${PLUGIN} SHARED
    ${SRC_DIR}/Plugin.cpp
    main.cpp
    CompanyFileSeparator.cpp
    CompanyParser.cpp
    main.cpp
    RecursiveParser.cpp
    RubricParser.cpp
    )

target_link_libraries(
    ${PLUGIN}
    dl
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    )

set(PLUGIN_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PLUGIN}")

#add_custom_command(
#    TARGET ${PLUGIN} POST_BUILD
#    COMMAND bash -c "if [ -d ${PLUGIN_DIR} ]\; then rm -r ${PLUGIN_DIR}\; fi\; mkdir ${PLUGIN_DIR}"
#    COMMAND cp "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/run" "${PLUGIN_DIR}"
#    COMMAND cp "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/lib${PLUGIN}.so" "${PLUGIN_DIR}"
#    DEPENDS ${RUN_BIN} ${PLUGIN}
#    )
