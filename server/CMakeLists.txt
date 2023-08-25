cmake_minimum_required(VERSION 2.6)

project(hash_server)

set(SRC_DIR src)
set(SOURCES ${SRC_DIR}/server.c  ${SRC_DIR}/multiprocessing.c  ${SRC_DIR}/handle_hash.c  ${SRC_DIR}/handle_soket.c  ${SRC_DIR}/handle_request.c)

add_executable(server.out ${SOURCES})

target_link_libraries(server.out mhash)
