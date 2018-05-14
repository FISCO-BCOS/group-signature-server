file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

find_package(Dev REQUIRED)
#find_package(json_rpc_cpp REQUIRED)

#include_directories(${ALGORITHM_INCLUDE_DIRS})
#include_directories(${DEVCORE_INCLUDE_DIRS})
#include_directories(${DATABASE_INCLUDE_DIRS})
#message(${DATABASE_INCLUDE_DIRS})
#message(${ALGORITHM_INCLUDE_DIRS})
#message(${DEVCORE_INCLUDE_DIRS})
include_directories(${JSONCPP_INCLUDE_DIRS})

add_library(http_server ${SRC_LIST} ${HEADERS})

target_link_libraries(http_server JsonRpcCpp::Server)
target_link_libraries(http_server group_sig)
target_link_libraries(http_server group_sig_devcore)
target_link_libraries(http_server database)

install(TARGETS http_server RUNTIME DESTINATION bin/group_sig/httpserver ARCHIVE DESTINATION lib/group_sig/httpserver LIBRARY DESTINATION lib/group_sig/httpserver)
INSTALL(FILES ${HEADERS} DESTINATION include/group_sig/httpserver) 
