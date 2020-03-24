file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

find_library(boost REQUIRED)

message(STATUS "#####BOOST INCLUDE:${BOOST_INCLUDE_DIR}")
include_directories(${BOOST_INCLUDE_DIR})

#list(APPEND HEADERS ${BOOST_INCLUDE_DIR})

add_library(Easylog ${SRC_LIST} ${HEADERS})
target_link_libraries(Easylog Boost::Thread pthread)

install(TARGETS Easylog RUNTIME DESTINATION bin/groupsig_client/easylog ARCHIVE DESTINATION lib/groupsig_client/easylog LIBRARY DESTINATION lib/groupsig_client/easylog)
INSTALL(FILES ${HEADERS} DESTINATION include/groupsig_client/easylog) 
