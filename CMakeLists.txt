cmake_minimum_required(VERSION 3.23)
project(OTIK_Archive)

set(CMAKE_CXX_STANDARD 14)

add_executable(Archiver  main.cpp Archiver.cpp functions.cpp)

target_link_libraries(Archiver "-static")
