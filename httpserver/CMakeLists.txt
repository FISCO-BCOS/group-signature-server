file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

#find_package(json_rpc_cpp REQUIRED)

#include_directories(${ALGORITHM_INCLUDE_DIRS})
#include_directories(${DEVCORE_INCLUDE_DIRS})
#include_directories(${DATABASE_INCLUDE_DIRS})
#message(${DATABASE_INCLUDE_DIRS})
#message(${ALGORITHM_INCLUDE_DIRS})
#message(${DEVCORE_INCLUDE_DIRS})
include_directories(${JSONCPP_INCLUDE_DIRS})

add_library(HttpServer ${SRC_LIST} ${HEADERS})

target_link_libraries(HttpServer Database JsonRpcCpp::Server JsonCpp Easylog GroupSig)

install(TARGETS HttpServer RUNTIME DESTINATION bin/groupsig_client/httpserver ARCHIVE DESTINATION lib/groupsig_client/httpserver LIBRARY DESTINATION lib/groupsig_client/httpserver)
INSTALL(FILES ${HEADERS} DESTINATION include/groupsig_client/httpserver) 
