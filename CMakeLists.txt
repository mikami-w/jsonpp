cmake_minimum_required(VERSION 3.20)
project(JSONpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(JSONpp
        tests/test.cpp
        src/jsonpp.cpp
        src/jsonpp.h
        src/jsonexception.h
        src/json_stream_adaptor.h
        src/stream_traits.h
)
