set(APP_NAME companyes-web)

add_executable(${APP_NAME}
    #SignaledMenuItem.cpp
    RubrickWidget.cpp
    RegionView.cpp
    RegionsCreator.cpp
    Main.cpp
    )

target_link_libraries(${APP_NAME}
    ${WTHTTP_LIBRARIES}
    ${WTEXT_LIBRARIES}
    ${WT_LIBRARIES}
    z
    crypto
    ssl
    pthread
    ${Boost_RANDOM_LIBRARY}
    ${Boost_SIGNALS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    )

# Копирование файла локализации
configure_file(${SRC_DIR}/companyes-web/rus_locale.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rus_locale.xml COPYONLY)

# Копирование файла настроек web текста
configure_file(${SRC_DIR}/companyes-web/main.css ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main.css COPYONLY)
