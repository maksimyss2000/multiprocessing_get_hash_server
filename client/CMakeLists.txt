cmake_minimum_required(VERSION 2.6)

project(hash_client)

set(SRC_DIR src)
set(SOURCES ${SRC_DIR}/client.c  ${SRC_DIR}/handle_request.c  ${SRC_DIR}/handle_response.c  ${SRC_DIR}/handle_soket.c)

add_executable(client.out ${SOURCES})

target_link_libraries(client.out)
