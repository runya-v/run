set(APP_NAME pleyfair)

add_executable(${APP_NAME}
    Cript.cpp
    UnCript.cpp
    main.cpp
    )

target_link_libraries(${APP_NAME}
    ${WTHTTP_LIBRARY}
    ${WTEXT_LIBRARY}
    ${WT_LIBRARY}
    ${Boost_RANDOM_LIBRARY}
    ${Boost_SIGNALS_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    pthread
    ssl
    )

# Копирование файла локализации
configure_file(${MAIN_SRC_DIR}/Pleyfair/rus_locale.xml ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/rus_locale.xml COPYONLY)

# Копирование файла настроек web текста
configure_file(${MAIN_SRC_DIR}/Pleyfair/main.css ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/main.css COPYONLY)
