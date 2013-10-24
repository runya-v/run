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
    -std=gnu++11
    -pedantic  -pedantic-errors
    -pipe -fPIC 
    -Wall -Wno-long-long -Wno-inline 
    -finline-functions -finline-limit=2000
    --param large-function-growth=5000 --param inline-unit-growth=600 
    -D__STDC_LIMIT_MACROS=1 -I.
    -D_GLIBCXX_USE_NANOSLEEP
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
    add_definitions(-O3 -ggdb3)
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")


# Установка вспомогательных констант
set(ROOT_DIR         ${CMAKE_CURRENT_SOURCE_DIR})
set(DATA_DIR         ${ROOT_DIR}/conf)
set(BOOST_DIR        ${ROOT_DIR}/boost)
set(SRC_DIR          ${ROOT_DIR}/src)
set(FOREXD_DIR       ${SRC_DIR}/forexd)
set(CLIENT_PROTO_DIR ${FOREXD_DIR}/client_protocol)
set(MIND_PROTO_DIR   ${FOREXD_DIR}/mind_protocol)
set(UTILS_DIR        ${SRC_DIR}/utils)
set(BINARY_DIR       ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)


# Поиск библиотек Boost версии старше 1.48
find_package(Boost 1.48 COMPONENTS
    date_time
    filesystem
    program_options
    system
    signals
    regex
    thread
    random
    unit_test_framework
    iostreams
    serialization
    )

if(Boost_FOUND)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_STATIC_RUNTIME OFF)
    set(BOOST_ALL_DYN_LINK ON)

    message(STATUS "Boost_LIBRARY_DIRS: \"${Boost_LIBRARY_DIRS}\"")
    message(STATUS "Boost_INCLUDE_DIRS: \"${Boost_INCLUDE_DIRS}\"")
    message(STATUS "Boost_LIBRARIES:")
    foreach(BOOST_LIB ${Boost_LIBRARIES})
        message(STATUS "    \"${BOOST_LIB}\"")
    endforeach()

    link_directories(${Boost_LIBRARY_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
    
    add_definitions(-DBOOST_DIN_LINK)
else(Boost_FOUND)
    message(FATAL_ERROR "Boost libraries not found. Please get a copy from http://www.boost.org")
endif(Boost_FOUND)

# Поиск библиотек MySQL
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(MYSQL)

if(MYSQL_FOUND)
    message(STATUS "MYSQL_INCLUDE_DIR: \"${MYSQL_INCLUDE_DIR}\"")
    message(STATUS "MYSQL_LIBRARIES: \"${MYSQL_LIBRARIES}\"")

    link_directories(${MYSQL_LIBRARIES})
    include_directories(${MYSQL_INCLUDE_DIR})
else(MYSQL_FOUND)
    message(FATAL_ERROR "MySQL libraries not found. Please install a mysql++3 library.")
endif(MYSQL_FOUND)


# Поиск cppcms библиотеки
find_path(CPPCMS_DIR NAMES "cppcms/application.h" PATHS "/usr/include" "/usr/local/include")
find_library(CPPCMS_LIBRARY NAMES cppcms libcppcms PATHS "/usr/lib" "/usr/local/lib")

if(${CPPCMS_LIBRARY} STREQUAL "CPPCMS_LIBRARY-NOTFOUND")
    message(FATAL_ERROR "CppCMS library is not found.")
else()
    message(STATUS "CPPCMS_DIR:         \"${CPPCMS_DIR}\"")
    message(STATUS "CPPCMS_LIBRARY:     \"${CPPCMS_LIBRARY}\"")
endif()


# Поиск htmlcxx библиотеки
find_path(HTMLCXX_DIR NAMES "html/ParserDom.h" "html/CharsetConverter.h" PATHS "/usr/include/htmlcxx" "/usr/local/include/htmlcxx")
find_library(HTMLCXX_LIBRARY NAMES htmlcxx libhtmlcxx PATHS "/usr/lib" "/usr/local/lib")

if(${HTMLCXX_LIBRARY} STREQUAL "HTMLCXX_LIBRARY-NOTFOUND")
    message(FATAL_ERROR "htmlcxx library is not found.")
else()
    message(STATUS "HTMLCXX_DIR:        \"${HTMLCXX_DIR}\"")
    message(STATUS "HTMLCXX_LIBRARY:    \"${HTMLCXX_LIBRARY}\"")
endif()

# Поиск WtCpp библиотеки
find_path(WT_DIR NAMES "Wt/WApplication" PATHS "/usr/include/Wt" "/usr/local/include/Wt")
find_library(WT_LIBRARY           NAMES wt           libwt           PATHS "/usr/lib" "/usr/local/lib")
find_library(WTHTTP_LIBRARY       NAMES wthttp       libwthttp       PATHS "/usr/lib" "/usr/local/lib")
find_library(WTEXT_LIBRARY        NAMES wtext        libwtext        PATHS "/usr/lib" "/usr/local/lib")
find_library(WTDBO_LIBRARY        NAMES wtdbo        libwdbo         PATHS "/usr/lib" "/usr/local/lib")
find_library(WTDBOSQLITE3_LIBRARY NAMES wtdbosqlite3 libwtdbosqlite3 PATHS "/usr/lib" "/usr/local/lib")

if(${WT_DIR} STREQUAL "WT_DIR-NOTFOUND")
    message(FATAL_ERROR "Wt C++ toolkit library is not found.")
else()
    message(STATUS "WT_DIR:              \"${WT_DIR}\"")
    message(STATUS "WT_LIBRARY:          \"${WT_LIBRARY}\"")
    message(STATUS "WTHTTP_LIBRARY       \"${WTHTTP_LIBRARY}\"")
    message(STATUS "WTEXT_LIBRARY        \"${WTEXT_LIBRARY}\"")
    message(STATUS "WTDBO_LIBRARY        \"${WTDBO_LIBRARY}\"")
    message(STATUS "WTDBOSQLITE3_LIBRARY \"${WTDBOSQLITE3_LIBRARY}\"")
endif()


# Указание общих настроек
link_directories("/usr/lib")
link_directories("/usr/local/lib")


# Установка общих директорий включаемых файлов
include_directories(${SRC_DIR})


# Элементы.
add_subdirectory("${SRC_DIR}")
add_subdirectory("${SRC_DIR}/layout-parser")
add_subdirectory("${SRC_DIR}/layout-server")
add_subdirectory("${SRC_DIR}/company-parser")
add_subdirectory("${SRC_DIR}/companyes-web")
add_subdirectory("${SRC_DIR}/http-server")
#add_subdirectory("${SRC_DIR}/ctpp2")


# Директорию test включается только, если определен макро BUILD_TESTS=1
if(BUILD_TESTS)
    message("Include tests directory to build process")

    set(TESTS_DIR ${SRC_DIR}/tests)
    message(STATUS "TESTS_DIR: ${TESTS_DIR}")

    enable_testing()

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${TESTS_DIR})
    include(UTest)

    add_subdirectory("${TESTS_DIR}")
else(BUILD_TESTS)
    message("Directory tests is not included to build. Run cmake -DBUILD_TESTS=1 to include.")
endif(BUILD_TESTS)
