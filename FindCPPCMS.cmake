# - Find mysqlclient
# Find the native CPPCMS includes and library
#
#  CPPCMS_INCLUDE_DIR - where to find *.h, etc.
#  CPPCMS_LIBRARIES   - List of libraries when using CPPCMS.
#  CPPCMS_FOUND       - True if CPPCMS found.

include(TakeComponent)
find_template(
    COMPONENT_NAME CPPCMS 
    INCLUDE_NAMES cppcms/application.h 
    LIB_NAMES cppcms libcppcms
    )
