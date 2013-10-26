# - Find mysqlclient
# Find the native HTMLCXX includes and library
#
#  HTMLCXX_INCLUDE_DIR - where to find *.h, etc.
#  HTMLCXX_LIBRARIES   - List of libraries when using HTMLCXX.
#  HTMLCXX_FOUND       - True if HTMLCXX found.

include(TakeComponent)
find_template(
    COMPONENT_NAME HTMLCXX 
    INCLUDE_NAMES htmlcxx/html/ParserDom.h htmlcxx/html/CharsetConverter.h 
    LIB_NAMES htmlcxx libhtmlcxx
    )
