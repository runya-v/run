set(APP_BIN http-server)
set(APP_LIB http_server)

add_library(${APP_LIB}
    Server.cpp
    RequestParser.cpp
    RequestHandler.cpp
    Reply.cpp
    MimeTypes.cpp
    Connection.cpp
    )

add_executable(${APP_BIN}
    main.cpp
    )

target_link_libraries(${APP_BIN}
    ${APP_LIB}
    pthread
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )


