cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_DIR})
set(GROUPSIG_CLIENT_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Path to the root directory for group sig")

set(LIBS Easylog HttpServer Database)
include_directories(${GROUPSIG_CLIENT_DIR})

include(CompilerSettings)
include(ProjectBoost)
include(ProjectJsonCpp)
include(ProjectJsonRpcCpp)
include(ProjectGroupSig)
include(ProjectLevelDB)
include(ProjectSnappy)

find_package(json_rpc_cpp REQUIRED)

add_subdirectory(easylog)
add_subdirectory(database)
add_subdirectory(httpserver)

add_executable(server server.cpp)

target_link_libraries(server pthread)
target_link_libraries(server GroupSig)
target_link_libraries(server Easylog)
target_link_libraries(server Database)
target_link_libraries(server HttpServer) 
target_link_libraries(server JsonRpcCpp::Server JsonCpp)
target_link_libraries(server Boost::Thread Boost::program_options Boost::Filesystem)
