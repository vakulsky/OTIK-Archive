cmake_minimum_required(VERSION 3.23)
project(OTIK_Archive)

set(CMAKE_CXX_STANDARD 14)

add_executable(Archiver main.cpp Archiver.cpp Shannon.cpp RLECompress.cpp LZ77Compress.cpp HammingCode.cpp ReedSolomonWrapper.cpp)

target_link_libraries(Archiver "-static")
