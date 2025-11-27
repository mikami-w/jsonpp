cmake_minimum_required(VERSION 3.20)
project(JSONpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(JSONpp
        tests/test.cpp
        src/jsonpp.cpp
        src/jsonpp.hpp
        src/jsonexception.hpp
        src/json_stream_adaptor.hpp
        src/stream_traits.hpp
)
