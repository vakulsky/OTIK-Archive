cmake_minimum_required(VERSION 3.23)
project(OTIK_Archive)

set(CMAKE_CXX_STANDARD 14)

add_executable(Archiver  main.cpp Archiver.cpp Packer.cpp Shannon.cpp RLECompress.cpp LZ77Compress.cpp LZ77Compress.h)

target_link_libraries(Archiver "-static")
