file(GLOB GROUP_SIG_SRC_LIST "*.cpp")
file(GLOB GROUP_SIG_HEADERS "*.h")

file(GLOB BBS04_SRC "bbs04/*.cpp")
file(GLOB BBS04_HEADERS "bbs04/*.h")

file(GLOB RING_SIG_SRC "ring-sig/*.cpp")
file(GLOB RING_SIG_HEADERS "ring-sig/*.h")

list(APPEND SRC_LIST ${BBS04_SRC})
list(APPEND HEADERS ${BBS04_HEADERS})


list(APPEND SRC_LIST ${GROUP_SIG_SRC_LIST})
list(APPEND HEADERS ${GROUP_SIG_HEADERS})

list(APPEND SRC_LIST ${RING_SIG_SRC})
list(APPEND HEADERS ${RING_SIG_HEADERS})

message(${SRC_LIST})
message(${HEADERS})
find_library(boost REQUIRED)
find_package(PBC REQUIRED)
find_package(Dev)

include_directories(goup_sig PUBLIC ${BOOST_INCLUDE_DIR})
#include_directories(${DEVCORE_INCLUDE_DIRS})
include_directories(${PBC_INCLUDE_DIR})
include_directories(${CRYPTOPP_INCLUDE_DIR})

add_library(group_sig ${SRC_LIST} ${HEADERS})

target_link_libraries(group_sig gmp pbc)
target_link_libraries(group_sig gmp pbc_sig)
target_link_libraries(group_sig group_sig_devcore)
target_link_libraries(group_sig Cryptopp)

install(TARGETS group_sig RUNTIME DESTINATION bin/group_sig/algorithm ARCHIVE DESTINATION lib/group_sig/algorithm LIBRARY DESTINATION lib/group_sig/algorithm)
INSTALL(FILES ${GROUP_SIG_HEADERS} DESTINATION include/group_sig/algorithm) 
INSTALL(FILES ${BBS04_HEADERS} DESTINATION include/group_sig/algorithm/bbs04) 
