# Процедура шаблон поиска компонента с множественными параметрами.
# Параметры:
#  find_template(COMPONENT INCLUDE_NAMES LIB_NAMES)
#   [COMPONENT_NAME] <NAME>       -- имя теста. 
#   [INCLUDE_NAMES]  <NAME1 ... > -- предпологаемые инклюд файлы.
#   [LIB_NAMES]      <NAME1 ... > -- предпологаемые имена библиотек.
# Подключение компонента разбора множественных аргументов
include(CMakeParseArguments)

macro(find_template)
    # Получение списков входных параметров
    set(OPTIONS ) 
    set(ONE_VALUE_ARGS COMPONENT_NAME) 
    set(MULTI_VALUE_ARGS INCLUDE_NAMES LIB_NAMES) 
    cmake_parse_arguments(GET "${OPTIONS}" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN}) 

    # Тело функции
    if(NOT ("${GET_COMPONENT_NAME}" STREQUAL ""))
        set(${GET_COMPONENT_NAME}_FOUND FALSE)
        
        find_path(
            ${GET_COMPONENT_NAME}_INCLUDE_DIR 
            NAMES ${GET_INCLUDE_NAMES}
            PATHS "/opt/include" "/usr/include" "/usr/local/include"
            DOC "Path where the CPPCMS header files can be found"
            )

        if(${GET_COMPONENT_NAME}_INCLUDE_DIR)
            find_library(
                ${GET_COMPONENT_NAME}_LIBRARIES 
                NAMES ${GET_LIB_NAMES} 
                PATHS "/lib" "/opt/lib" "/usr/lib" "/usr/local/lib"
                DOC "Path where the CPPCMS libraries can be found"
                )
              
            if(${GET_COMPONENT_NAME}_LIBRARIES)
                set(${GET_COMPONENT_NAME}_FOUND TRUE)
                message(STATUS "`${GET_COMPONENT_NAME}` hash function support enabled")
            else(${GET_COMPONENT_NAME}_LIBRARIES)
                message(STATUS "ERROR: Cannot find `${GET_COMPONENT_NAME}`, aborting")
            endif(${GET_COMPONENT_NAME}_LIBRARIES)
        elseif(${GET_COMPONENT_NAME}_INCLUDE_DIR)
            message(STATUS "ERROR: Cannot find `${GET_COMPONENT_NAME}`, aborting")
        endif(${GET_COMPONENT_NAME}_INCLUDE_DIR)
    endif(NOT ("${GET_COMPONENT_NAME}" STREQUAL ""))
endmacro(find_template)


# Процедура поиска внешнего компонента и добавления его в сборку.
# Параметры:
#  take_component(COMPONENT)
#   COMPONENT -- имя компонента.
function(take_component COMPONENT)
    find_package(${COMPONENT})

    if(${COMPONENT}_FOUND)
        message(STATUS "${COMPONENT}_INCLUDE_DIR: \"${${COMPONENT}_INCLUDE_DIR}\"")
        include_directories(${${COMPONENT}_INCLUDE_DIR})

        message(STATUS "${COMPONENT}_LIBRARIES:")
        foreach(LIB ${${COMPONENT}_LIBRARIES})
            message(STATUS "    \"${LIB}\"")
        endforeach()
        link_directories(${${COMPONENT}_LIBRARIES})
    else(${COMPONENT}_FOUND)
        message(FATAL_ERROR "ERROR: libraries not found. Please install a ${COMPONENT} library.")
    endif(${COMPONENT}_FOUND)
endfunction(take_component)
