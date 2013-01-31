set(APP_NAME companyes-web)

add_executable(${APP_NAME}
    RubricksCreator.cpp
    RubrickView.cpp
    Main.cpp
    )

target_link_libraries(${APP_NAME}
    ${WTCPP_LIBRARY}
    ${WTCPPHTTP_LIBRARY}
    ${Boost_SIGNALS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    )

# Копирование файла локализации
configure_file(${MAIN_SRC_DIR}/companyes-web/rus_locale.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rus_locale.xml COPYONLY)

# Копирование файла настроек web текста
configure_file(${MAIN_SRC_DIR}/companyes-web/main.css ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main.css COPYONLY)
