# Настройка конфигурационных макросов
message(STATUS "Set BOOST_ASIO_ENABLE_SEQUENTIAL_STRAND_ALLOCATION")
message(STATUS "Set BOOST_ASIO_STRAND_IMPLEMENTATIONS = 10 000")
add_definitions(
    -DBOOST_ASIO_ENABLE_SEQUENTIAL_STRAND_ALLOCATION 
    -DBOOST_ASIO_STRAND_IMPLEMENTATIONS=10000
    )

set(APP_LIB http_server)
add_library(${APP_LIB}
    Server.cpp
    RequestParser.cpp
    RequestHandler.cpp
    Reply.cpp
    MimeTypes.cpp
    Connection.cpp
    HtmlMaker.cpp
    )


add_subdirectory("${HTTP_SERVER_DIR}/templating")
include_directories("${HTTP_SERVER_DIR}/templating")


set(APP_BIN http-server)
add_executable(${APP_BIN}
    main.cpp
    )
target_link_libraries(${APP_BIN}
    ${APP_LIB}
    templating
    ctpp2
    base
    pthread
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_IOSTREAMS_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    )

if(ENABLE_TESTS)
    add_unit_test(ut_ctpp_html base http_server templating ctpp2 ${LOG_LIBS})
endif(ENABLE_TESTS)

