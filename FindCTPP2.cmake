# - Find mysqlclient
# Find the native CPPCMS includes and library
#
#  CTPP2_INCLUDE_DIR - where to find *.h, etc.
#  CTPP2_LIBRARIES   - List of libraries when using CPPCMS.
#  CTPP2_FOUND       - True if CPPCMS found.

include(TakeComponent)
find_template(
    COMPONENT_NAME CTPP2 
    INCLUDE_NAMES ctpp2/CDT.hpp 
    LIB_NAMES ctpp2 libctpp2
    )
