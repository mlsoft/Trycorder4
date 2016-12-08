cmake_minimum_required(VERSION 2.6)
project(tryserver)

add_executable(tryserver main.cpp)

install(TARGETS tryserver RUNTIME DESTINATION bin)
