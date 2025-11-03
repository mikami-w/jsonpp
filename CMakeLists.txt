cmake_minimum_required(VERSION 4.0)
project(JSONpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(JSONpp
        main.cpp
        jsonpp.cpp
        jsonpp.h
        jsonexception.h
        basic_json_stream.h
)

