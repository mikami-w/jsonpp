cmake_minimum_required(VERSION 4.0)
project(JSONpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(JSONpp
        main.cpp
        jsonpp.cpp
        jsonpp.h
)
