file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

find_library(boost REQUIRED)
find_package(PBC REQUIRED)

message(STATUS "#####BOOST INCLUDE:${BOOST_INCLUDE_DIR}")
include_directories(${BOOST_INCLUDE_DIR})
include_directories(${PBC_INCLUDE_DIR})

#list(APPEND HEADERS ${BOOST_INCLUDE_DIR})

add_library(group_sig_devcore ${SRC_LIST} ${HEADERS})
target_link_libraries(group_sig_devcore gmp pbc)
target_link_libraries(group_sig_devcore gmp pbc_sig)
target_link_libraries(group_sig_devcore Boost::Filesystem)
target_link_libraries(group_sig_devcore Boost::Thread)
target_link_libraries(group_sig_devcore pthread)

install(TARGETS group_sig_devcore RUNTIME DESTINATION bin/group_sig/devcore ARCHIVE DESTINATION lib/group_sig/devcore LIBRARY DESTINATION lib/group_sig/devcore)
INSTALL(FILES ${HEADERS} DESTINATION include/group_sig/devcore) 
