set(TEST_LIB unit_test)
add_library(${TEST_LIB}
    OptionsEmulator.cpp
    #ClientEmulator.cpp
    Killer.cpp
    SaveConfigFile.cpp
    SaveInstrumentsFile.cpp
    SaveUserFile.cpp
    SaveAllowsFile.cpp
    )


set(LOG_LIBS z ${Boost_FILESYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})
set(FOREXD_LIBS forex client_protocol mind_protocol)


# Пример добавления теста:
# add_unit_tests( <Имя теста> \t <библиотека 1>  ... <библиотека N>)
d_unit_tests( ut_thread_pool_performance base pthread ${LOG_LIBS} ${Boost_SYSTEM_LIBRARY})
