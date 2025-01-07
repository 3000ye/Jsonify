cmake_minimum_required(VERSION 3.30)
project(Jsonify)

set(CMAKE_CXX_STANDARD 17)

add_executable(
    Jsonify jsonify.cpp jsonify.hpp test.cpp
)
