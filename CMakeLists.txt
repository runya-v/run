cmake_minimum_required(VERSION 2.8)

message("\nConfigure project Run...")

project(Run)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE
        )
endif(NOT CMAKE_BUILD_TYPE)

# Общие флаги для всех сборок.
add_definitions(
    -std=c++0x
    -pipe -fPIC -Wall
    -D__STDC_LIMIT_MACROS=1 -I.
    -DDLL_PUBLIC=__attribute__\(\(visibility\(\"default\"\)\)\)
    )

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    #Добавление профилировки: для включения профилировки запускаем cmake -DGPROF=1.
    if(GPROF)
        add_definitions(-pg)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    endif(GPROF)

    #Добавляем флаги для построения отладочной инфомрации.
    add_definitions(-fno-inline-functions -fno-inline)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-O2 -ggdb3)
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

set(MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(ROOT ${BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ROOT}/${CMAKE_BUILD_TYPE}/lib)

message("-- MAIN_SRC_DIR:                   ${MAIN_SRC_DIR}")
message("-- BINARY_DIR:                     ${BINARY_DIR}")
message("-- ROOT:                           ${ROOT}")
message("-- CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("-- CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message("-- CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")

# Устанавливаем зависимости на boost.
set(Boost_USE_MULTITHREADED ON)

find_package(Boost 1.49.0 COMPONENTS
    date_time
    filesystem
    program_options
    system
    signals
    regex
    thread
    unit_test_framework
    )

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message("-- Boost_INCLUDE_DIRS: \"${Boost_INCLUDE_DIRS}\"")
else()
    message(FATAL_ERROR "Boost libraries not found. Please get a copy from http://www.boost.org")
endif()

link_directories("/usr/lib")
link_directories("/usr/local/lib")

include_directories(${MAIN_SRC_DIR})

# Поиск cppcms библиотеки
find_path(CPPCMS_DIR NAMES "cppcms/application.h" PATHS "/usr/include" "/usr/local/include")
find_library(CPPCMS_LIBRARY NAMES cppcms libcppcms PATHS "/usr/lib" "/usr/local/lib")

if(${CPPCMS_LIBRARY} STREQUAL "CPPCMS_LIBRARY-NOTFOUND")
    message(FATAL_ERROR "CppCMS library is not found.")
else()
    message("-- CPPCMS_DIR:         \"${CPPCMS_DIR}\"")
    message("-- CPPCMS_LIBRARY:     \"${CPPCMS_LIBRARY}\"")
endif()

# Поиск htmlcxx библиотеки
find_path(HTMLCXX_DIR NAMES "html/ParserDom.h" "html/CharsetConverter.h" PATHS "/usr/include/htmlcxx" "/usr/local/include/htmlcxx")
find_library(HTMLCXX_LIBRARY NAMES htmlcxx libhtmlcxx PATHS "/usr/lib" "/usr/local/lib")

if(${HTMLCXX_LIBRARY} STREQUAL "HTMLCXX_LIBRARY-NOTFOUND")
    message(FATAL_ERROR "htmlcxx library is not found.")
else()
    message("-- HTMLCXX_DIR:        \"${HTMLCXX_DIR}\"")
    message("-- HTMLCXX_LIBRARY:    \"${HTMLCXX_LIBRARY}\"")
endif()

# Поиск WtCpp библиотеки
find_path(WTCPP_DIR NAMES "Wt/WApplication" PATHS "/usr/include/Wt" "/usr/local/include/Wt")
find_library(WTCPP_LIBRARY NAMES wt libwt PATHS "/usr/lib" "/usr/local/lib")
find_library(WTCPPHTTP_LIBRARY NAMES wthttp libwthttp PATHS "/usr/lib" "/usr/local/lib")
find_library(WTCPPEXT_LIBRARY NAMES wtext libwtext PATHS "/usr/lib" "/usr/local/lib")

if(${WTCPP_DIR} STREQUAL "WTCPP_DIR-NOTFOUND")
    message(FATAL_ERROR "htmlcxx library is not found.")
else()
    message("-- WTCPP_DIR:          \"${WTCPP_DIR}\"")
    message("-- WTCPP_LIBRARY:      \"${WTCPP_LIBRARY}\"")
    message("-- WTCPPHTTP_LIBRARY   \"${WTCPPHTTP_LIBRARY}\"")
    message("-- WTCPPEXT_LIBRARY    \"${WTCPPEXT_LIBRARY}\"")
endif()

# Элементы.
add_subdirectory("${MAIN_SRC_DIR}")
add_subdirectory("${MAIN_SRC_DIR}/layout-parser")
add_subdirectory("${MAIN_SRC_DIR}/layout-server")
add_subdirectory("${MAIN_SRC_DIR}/company-parser")
add_subdirectory("${MAIN_SRC_DIR}/companyes-web")
add_subdirectory("${MAIN_SRC_DIR}/http-server")

# Директорию test включается только, если определен макро BUILD_TESTS=1
if(BUILD_TESTS)
    message("Include tests directory to build process")

    set(TEST_DIR ${MAIN_SRC_DIR}/test)
    message("-- TEST_DIR:                   ${TEST_DIR}")

    add_subdirectory("${TEST_DIR}")
else(BUILD_TESTS)
    message("Directory tests is not included to build. Run cmake -DBUILD_TESTS=1 to include.")
endif(BUILD_TESTS)

# Директорию utest включается только, если определен макро BUILD_UTESTS=1
if(BUILD_UTESTS)
    message("Include unit tests directory to build process")
    enable_testing()

    set(UTEST_DIR ${MAIN_SRC_DIR}/utest)
    message("-- UTEST_DIR:                   ${UTEST_DIR}")

    add_subdirectory("${UTEST_DIR}")
else(BUILD_UTESTS)
    message("Directory utest is not included to build. Run cmake -DBUILD_UTESTS=1 to include.")
endif(BUILD_UTESTS)
