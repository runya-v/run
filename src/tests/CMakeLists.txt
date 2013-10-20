set(LOG_LIBS z ${Boost_FILESYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})


# Пример добавления теста:
# add_unit_tests(<Имя теста> <библиотека 1>  ... <библиотека N>)
add_unit_test(ut_thread_pool_performance base pthread ${LOG_LIBS} ${Boost_SYSTEM_LIBRARY})
