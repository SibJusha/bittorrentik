cmake_minimum_required(VERSION 3.20)

project(bittorrentik)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp src/*.ixx src/*.ipp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(CXX_FLAGS "${CXX_FLAGS} --experimental-modules-support")

add_executable(bittorrentik ${SOURCE_FILES})
