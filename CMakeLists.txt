cmake_minimum_required(VERSION 3.24)
project(JSONpp)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 定义库 INTERFACE
add_library(jsonpp_lib INTERFACE)
target_include_directories(jsonpp_lib INTERFACE src)

# 2. 引入 GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

# 3. 自动化测试构建
# 在这里列出所有的测试源文件
# 每当你添加新测试，只需要在这里加一行文件名即可！
set(GTEST_SOURCES
        tests/gtest_correctness.cpp
        tests/gtest_usage.cpp
        tests/gtest_roundtrip.cpp
        tests/gtest_spotcheck.cpp
)

foreach(test_src ${GTEST_SOURCES})
    # 1. 从路径中提取文件名作为测试名 (e.g. "tests/gtest_usage.cpp" -> "gtest_usage")
    get_filename_component(test_name ${test_src} NAME_WE)

    # 2. 创建可执行文件
    add_executable(${test_name} ${test_src})

    # 3. 链接 GTest 和 库
    # 链接 jsonpp_lib 会自动处理头文件包含路径
    target_link_libraries(${test_name} PRIVATE GTest::gtest_main jsonpp_lib)

    # 4. 注册测试到 CTest
    gtest_discover_tests(${test_name})

    message(STATUS "Added test target: ${test_name}")
endforeach()

add_executable(test_parsing_serializing
        tests/manual_validation_tests/test_parsing_serializing.cpp

)