file(GLOB DB_SRC_LIST "*.cpp")
file(GLOB DB_HEADERS "*.h")

file(GLOB LEVELDB_LIST "levelDB/*.cpp")
file(GLOB LEVELDB_HEADERS "levelDB/*.h")

list(APPEND SRC_LIST ${DB_SRC_LIST})
list(APPEND HEADERS ${DB_HEADERS})

list(APPEND SRC_LIST ${LEVELDB_LIST})
list(APPEND HEADERS ${LEVELDB_HEADERS})

message("LEVELDB")
message(${SRC_LIST})
message(${HEADERS})

find_package(Dev)
find_package(LevelDB)

include_directories(${LEVELDB_INCLUDE_DIR})
#include_directories(${DEVCORE_INCLUDE_DIRS})

add_library(database ${SRC_LIST} ${HEADERS})
target_link_libraries(database leveldb)
target_link_libraries(database group_sig_devcore)

install(TARGETS database RUNTIME DESTINATION bin/group_sig/database ARCHIVE DESTINATION lib/group_sig/database LIBRARY DESTINATION lib/group_sig/database)
INSTALL(FILES ${DB_HEADERS} DESTINATION include/group_sig/database) 
INSTALL(FILES ${LEVELDB_HEADERS} DESTINATION include/group_sig/database/levelDB/) 
