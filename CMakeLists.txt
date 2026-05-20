cmake_minimum_required(VERSION 3.10)
project(LDCF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(ldcf 
    src/main.cpp
    src/bucket.cpp
    src/hash.cpp
    src/level.cpp
    src/ldcf.cpp
    src/genome.cpp
    src/kmer.cpp
)