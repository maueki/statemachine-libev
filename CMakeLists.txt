cmake_minimum_required(VERSION 2.8)
project (test_project)
add_executable(statemachine src/main.cpp)

target_link_libraries(statemachine -lev -lpthread)
