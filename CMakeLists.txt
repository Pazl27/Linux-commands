cmake_minimum_required(VERSION 3.21)
project(BusyBox C)

set(CMAKE_C_STANDARD 11)

add_executable(BusyBox src/main.c
        src/ls.h
        src/ls.c
        src/cat.h
        src/cat.c)

add_custom_command(
        TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/BusyBox
        ${CMAKE_CURRENT_BINARY_DIR}/cat)
add_custom_command(
        TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/BusyBox
        ${CMAKE_CURRENT_BINARY_DIR}/ls)
add_custom_command(
        TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/BusyBox
        ${CMAKE_CURRENT_BINARY_DIR}/grep)
add_custom_command(
        TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/BusyBox
        ${CMAKE_CURRENT_BINARY_DIR}/wc)
add_custom_command(
        TARGET ${PROJECT_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/BusyBox
        ${CMAKE_CURRENT_BINARY_DIR}/sort)
