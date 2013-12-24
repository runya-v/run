# Пример добавления теста:
# add_unit_tests(<Имя теста> <библиотека 1>  ... <библиотека N>)

add_unit_test(ut_thread_pool_performance base pthread ${LOG_LIBS})
add_unit_test(ut_log_completer base ${LOG_LIBS})
