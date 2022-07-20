cmake_minimum_required(VERSION 3.16)
project(segments_game)

set(CMAKE_CXX_STANDARD 14)

set(SG_SOURCES main.cpp)

add_executable(sg_game ${SG_SOURCES})

install(TARGETS sg_game ARCHIVE DESTINATION bin)
