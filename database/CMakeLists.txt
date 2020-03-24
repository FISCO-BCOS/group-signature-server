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

include_directories(${LEVELDB_INCLUDE_DIR})

add_library(Database ${SRC_LIST} ${HEADERS})
target_link_libraries(Database LevelDB Easylog GroupSig)

install(TARGETS Database RUNTIME DESTINATION bin/groupsig_client/database ARCHIVE DESTINATION lib/groupsig_client/database LIBRARY DESTINATION lib/groupsig_client/database)
INSTALL(FILES ${DB_HEADERS} DESTINATION include/groupsig_client/database) 
INSTALL(FILES ${LEVELDB_HEADERS} DESTINATION include/groupsig_client/database/levelDB/) 
