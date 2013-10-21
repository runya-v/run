set(BASE_LIB base)
add_library(${BASE_LIB}
    Exception.cpp
    Log.cpp
    Settings.cpp
    ThreadPool.cpp
    Task.cpp
    )

set(RUN_BIN run)
add_executable(${RUN_BIN}
    Plugin.cpp
    main.cpp
    )
target_link_libraries(${RUN_BIN}
    dl
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )

#set(SETTINGS "settings")
#add_library(${SETTINGS} SHARED Settings.cpp Plugin.cpp)
#add_custom_command(
#    TARGET ${SETTINGS} POST_BUILD
#    COMMAND mv "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libsettings.so" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
