cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_DIR})
set(GROUP_SIG_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Path to the root directory for group sig")


include(CompilerSettings)
include(ProjectBoost)
include(ProjectJsonCpp)
include(ProjectJsonRpcCpp)
include(ProjectCryptopp)
find_package(Dev)
find_package(json_rpc_cpp REQUIRED)

include_directories(${BOOST_INCLUDE_DIR})
include_directories(${JSONCPP_INCLUDE_DIRS})
include_directories(${GROUP_SIG_INCLUDE_DIRS})
include_directories(${CRYPTOPP_INCLUDE_DIR})
include_directories(${LEVELDB_INCLUDE_DIR})
#include_directories(${DEVCORE_INCLUDE_DIRS})
#include_directories(${SERVER_INCLUDE_DIRS})
#include_directories(${ALGORITHM_INCLUDE_DIRS})
#include_directories(${DATABASE_INCLUDE_DIRS})

add_subdirectory(devcore)
add_subdirectory(algorithm)
add_subdirectory(database)
add_subdirectory(httpserver)

add_executable(server server.cpp)

target_link_libraries(server group_sig_devcore)
target_link_libraries(server group_sig)
target_link_libraries(server database)
target_link_libraries(server http_server)
target_link_libraries(server JsonRpcCpp::Server)
target_link_libraries(server Cryptopp)
target_link_libraries(server Boost::Thread)
target_link_libraries(server Boost::program_options)
target_link_libraries(server pthread)

