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
set(UTILS_DIR        ${SRC_DIR}/utils)
set(BINARY_DIR       ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)


# Указание общих настроек
link_directories("/usr/lib")
link_directories("/usr/local/lib")


# Установка общих директорий включаемых файлов
include_directories(${SRC_DIR})


# Настройка опций сборки компонент
OPTION(ENABLE_MYSQL   "Enable MYSQL support [default: ON]"   OFF)
OPTION(ENABLE_CPPCMS  "Enable CPPCMS support [default: ON]"  OFF)
OPTION(ENABLE_HTMLCXX "Enable HTMLCXX support [default: ON]" OFF)
OPTION(ENABLE_CTPP2   "Enable CTPP2 support [default: ON]"   ON)
OPTION(ENABLE_WTCPP   "Enable WTCPP support [default: ON]"   OFF)
OPTION(ENABLE_TESTS   "Enable TESTS support [default: ON]"   ON)


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


# Поиск дополнительных библиотек
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}")
include(TakeComponent)
 

# Включение в сборку компонент
# Директорию test включается только, если определен макро ENABLE_TESTS=ON
add_subdirectory("${SRC_DIR}")
add_subdirectory("${SRC_DIR}/company-parser")

set(LOG_LIBS z ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})

if(ENABLE_TESTS)
    message("Include tests directory to build process")

    set(TESTS_DIR ${SRC_DIR}/tests)
    message(STATUS "TESTS_DIR: ${TESTS_DIR}")

    enable_testing()

    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${TESTS_DIR})
    include(UTest)

    add_subdirectory("${TESTS_DIR}")
else(ENABLE_TESTS)
    message("Directory tests is not included to build. Run cmake -DENABLE_TESTS=ON to include.")
endif(ENABLE_TESTS)

if(ENABLE_CTPP2)
    take_component(CTPP2)
    set(HTTP_SERVER_DIR  ${SRC_DIR}/http-server)
    add_subdirectory("${HTTP_SERVER_DIR}")
endif(ENABLE_CTPP2)

if(ENABLE_WTCPP)
    take_component(WTCPP)
    add_subdirectory("${SRC_DIR}/companyes-web")
endif(ENABLE_WTCPP)

if(ENABLE_CPPCMS)
    take_component(CPPCMS)
endif(ENABLE_CPPCMS)

if(ENABLE_HTMLCXX)
    take_component(HTMLCXX)
endif(ENABLE_HTMLCXX)

if(ENABLE_CPPCMS AND ENADLE_HTMLCXX)
    add_subdirectory("${SRC_DIR}/layout-parser")
    add_subdirectory("${SRC_DIR}/layout-server")
endif(ENABLE_CPPCMS AND ENADLE_HTMLCXX)

if(ENABLE_MYSQL)
    take_component(MYSQL)
endif(ENABLE_MYSQL)
