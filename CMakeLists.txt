# CMakeLists file
cmake_minimum_required(VERSION 2.6)

project (kdb)

INCLUDE_DIRECTORIES(
  /usr/local/include/knet/
  ${PROJECT_SOURCE_DIR}/interface
  ${PROJECT_SOURCE_DIR}/internal/server
  ${PROJECT_SOURCE_DIR}/internal/protocol
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
LINK_DIRECTORIES(/usr/local/lib)

SET(CMAKE_C_FLAGS "-g -O2 -Wall")

add_executable(kdb
	internal/server/db_server.c
	internal/server/db_space.c
	internal/server/db_util.c
	internal/server/db_queue.c
	internal/server/db_task.c
	internal/server/db_worker.c
	internal/server/test.c
	internal/protocol/memcache_analyzer.c
)

target_link_libraries(kdb -lknet -lpthread -ldl)
INSTALL(TARGETS kdb DESTINATION bin)
