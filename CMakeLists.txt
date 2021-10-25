cmake_minimum_required(VERSION 3.17)
project(ordresemaphore C)

set(CMAKE_C_STANDARD 11)

add_executable(ordresemaphore main.c)
target_link_libraries(${PROJECT_NAME} pthread)