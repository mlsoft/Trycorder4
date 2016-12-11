cmake_minimum_required(VERSION 2.6)
project(tryserver)

add_executable(tryserver main.cpp)

target_link_libraries(tryserver PUBLIC pthread)

install(TARGETS tryserver RUNTIME DESTINATION bin)
